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

int64_t RingBuffer::enqueue(const char* json_payload, uint32_t len, const char* signature_hex, uint8_t priority) {
    // 1. Backpressure: Check if buffer is completely full BEFORE modifying head_
    if (head_.load(std::memory_order_relaxed) - tail_.load(std::memory_order_relaxed) >= capacity_) {
        return -1; // HTTP layer should return 429 Too Many Requests
    }

    // 2. Atomically claim the next write position
    uint64_t current_head = head_.fetch_add(1, std::memory_order_relaxed);

    // 3. Fast wrap-around modulo calculation
    size_t index = current_head & (capacity_ - 1);
    TransactionSlot& slot = slots_[index];

    // 4. Spin-wait until the slot is truly EMPTY, and lock it by setting it to WRITTEN
    SlotState expected = SlotState::EMPTY;
    while (!slot.state.compare_exchange_weak(expected, SlotState::WRITTEN, std::memory_order_acquire)) {
        expected = SlotState::EMPTY;
        #ifdef _WIN32
            YieldProcessor();
        #else
            _mm_pause();
        #endif
    }

    // 5. Write the payload data straight into the locked memory slot
    std::memcpy(slot.payload, json_payload, std::min(static_cast<size_t>(len), MAX_PAYLOAD_SIZE));
    
    // TEAMMATE: Storing the signature inside the slot so the dispatcher can verify it later
    if (signature_hex) {
        std::strncpy(slot.signature, signature_hex, sizeof(slot.signature) - 1);
        slot.signature[sizeof(slot.signature) - 1] = '\0';
    }
    slot.payload_len = len;
    slot.priority = priority;
    slot.ingested_at_us = now_microseconds();
    slot.slot_id = current_head;

    // 6. Release the slot to the Crypto workers
    slot.state.store(SlotState::PROCESSING, std::memory_order_release);
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