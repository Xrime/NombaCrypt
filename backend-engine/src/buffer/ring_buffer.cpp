#include "../../include/buffer/ring_buffer.hpp"
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <emmintrin.h>
#endif

namespace nombacrypt {

RingBuffer::RingBuffer(size_t capacity) : capacity_(capacity) {
    // Capacity must be a power of 2 for the bitwise AND modulo trick to work.
}

int64_t RingBuffer::enqueue(const char* json_payload, uint32_t len, uint8_t priority) {
    // 1. Atomically claim the next write position
    uint64_t current_head = head_.fetch_add(1, std::memory_order_relaxed);
    uint64_t current_tail = tail_.load(std::memory_order_acquire);

    // 2. Backpressure: Check if buffer is completely full
    if (current_head - current_tail >= capacity_) {
        head_.fetch_sub(1, std::memory_order_relaxed); // Revert claim
        return -1; // HTTP layer should return 503 Service Unavailable
    }

    // 3. Fast wrap-around modulo calculation
    size_t index = current_head & (capacity_ - 1);
    TransactionSlot& slot = slots_[index];

    // 4. Spin-wait until the slot is truly EMPTY (in case tail cleanup is lagging)

    SlotState expected = SlotState::EMPTY;
    while (!slot.state.compare_exchange_weak(expected, SlotState::EMPTY, std::memory_order_relaxed)) {
        expected = SlotState::EMPTY;
        #ifdef _WIN32
            YieldProcessor();
        #else
            _mm_pause();
        #endif
    }

    // 5. Write the payload data straight into the pre-allocated memory slot
    std::memcpy(slot.payload, json_payload, std::min(static_cast<size_t>(len), MAX_PAYLOAD_SIZE));
    slot.payload_len = len;
    slot.priority = priority;
    slot.ingested_at_us = now_microseconds();
    slot.slot_id = current_head;

    // 6. Release the slot to the Crypto workers for security verification
    //    Flow: WRITTEN → (CryptoWorker verifies) → PROCESSING → DISPATCHED
    slot.state.store(SlotState::WRITTEN, std::memory_order_release);
    total_enqueued_.fetch_add(1, std::memory_order_relaxed);

    return index;
}

TransactionSlot* RingBuffer::dequeue_for_processing() {
    uint64_t current_tail = tail_.load(std::memory_order_relaxed);
    uint64_t current_head = head_.load(std::memory_order_acquire);

    // Buffer is totally empty
    if (current_tail == current_head) return nullptr;

    size_t index = current_tail & (capacity_ - 1);
    TransactionSlot& slot = slots_[index];

    // Only dequeue if the Crypto thread has approved it (changed state to PROCESSING)
    if (slot.state.load(std::memory_order_acquire) == SlotState::PROCESSING) {
        return &slot;
    }

    return nullptr;
}

TransactionSlot* RingBuffer::dequeue_for_crypto_verification() {
    uint64_t current_tail = tail_.load(std::memory_order_relaxed);
    uint64_t current_head = head_.load(std::memory_order_acquire);

    // Scan from tail towards head looking for a WRITTEN slot
    for (uint64_t pos = current_tail; pos < current_head; ++pos) {
        size_t index = pos & (capacity_ - 1);
        TransactionSlot& slot = slots_[index];

        // Try to atomically claim a WRITTEN slot
        SlotState expected = SlotState::WRITTEN;
        if (slot.state.compare_exchange_strong(
                expected, SlotState::WRITTEN,
                std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return &slot;
        }
    }
    return nullptr;
}

void RingBuffer::mark_dispatched(int64_t slot_id) {
    // Apply the fast bitwise modulo to wrap the huge ID back into the 0-8191 range!
    size_t index = slot_id & (capacity_ - 1);
    TransactionSlot& slot = slots_[index];

    // Wipe the slot clean
    slot.state.store(SlotState::EMPTY, std::memory_order_release);

    // Move the tail forward to officially free the space
    tail_.fetch_add(1, std::memory_order_release);
    total_dispatched_.fetch_add(1, std::memory_order_relaxed);
}

BufferTelemetry RingBuffer::get_telemetry() const {
    uint64_t current_head = head_.load(std::memory_order_relaxed);
    uint64_t current_tail = tail_.load(std::memory_order_relaxed);

    size_t active = current_head - current_tail;
    float pct = (static_cast<float>(active) / capacity_) * 100.0f;

    return {
        active,
        pct,
        total_enqueued_.load(std::memory_order_relaxed),
        total_dispatched_.load(std::memory_order_relaxed)
    };
}

} // namespace nombacrypt