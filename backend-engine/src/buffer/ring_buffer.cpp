/**
 * @file ring_buffer.cpp
 * @brief Implementation of nombacrypt::RingBuffer.
 */

#include "buffer/ring_buffer.hpp"

#include <chrono>

namespace nombacrypt {

// ── Constructor ──────────────────────────────────────────────────────────────

RingBuffer::RingBuffer() {
    for (std::size_t i = 0; i < BUFFER_CAPACITY; ++i) {
        slots_[i].reset();
        slots_[i].slot_id = i;
    }
}

// ── Enqueue ──────────────────────────────────────────────────────────────────

std::optional<uint64_t> RingBuffer::enqueue(const char* data, uint32_t len, uint8_t priority) {
    // Attempt to claim the next head slot.
    uint64_t pos = head_.load(std::memory_order_relaxed);
    for (;;) {
        auto& slot = slots_[mask(pos)];
        uint8_t expected = SLOT_EMPTY;
        if (!slot.state.compare_exchange_weak(expected, SLOT_WRITTEN,
                                               std::memory_order_acq_rel)) {
            // Slot is not empty — buffer may be full.
            if (pos - tail_.load(std::memory_order_acquire) >= BUFFER_CAPACITY) {
                return std::nullopt; // truly full
            }
            pos = head_.load(std::memory_order_relaxed);
            continue;
        }
        // We own the slot — write payload.
        head_.compare_exchange_strong(pos, pos + 1, std::memory_order_release);
        slot.slot_id = pos;
        slot.write(data, len, priority);

        // Record ingestion time.
        auto now = std::chrono::steady_clock::now();
        slot.ingested_at_us = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch()).count());

        slot.state.store(SLOT_WRITTEN, std::memory_order_release);
        total_enqueued_.fetch_add(1, std::memory_order_relaxed);
        return mask(pos);
    }
}

// ── Dequeue for processing ───────────────────────────────────────────────────

TransactionSlot* RingBuffer::dequeue_for_processing() {
    uint64_t pos = tail_.load(std::memory_order_relaxed);
    for (;;) {
        if (pos >= head_.load(std::memory_order_acquire)) {
            return nullptr; // nothing to read
        }
        auto& slot = slots_[mask(pos)];
        uint8_t expected = SLOT_WRITTEN;
        if (slot.state.compare_exchange_weak(expected, SLOT_PROCESSING,
                                              std::memory_order_acq_rel)) {
            tail_.compare_exchange_strong(pos, pos + 1, std::memory_order_release);
            return &slot;
        }
        // Another consumer won this slot — try next.
        pos = tail_.load(std::memory_order_relaxed);
    }
}

// ── Mark dispatched ──────────────────────────────────────────────────────────

void RingBuffer::mark_dispatched(uint64_t index) {
    auto& slot = slots_[mask(index)];
    slot.state.store(SLOT_DISPATCHED, std::memory_order_release);
    total_dispatched_.fetch_add(1, std::memory_order_relaxed);
}

// ── Recycle ──────────────────────────────────────────────────────────────────

uint64_t RingBuffer::recycle_dispatched() {
    uint64_t count = 0;
    for (std::size_t i = 0; i < BUFFER_CAPACITY; ++i) {
        uint8_t expected = SLOT_DISPATCHED;
        if (slots_[i].state.compare_exchange_strong(expected, SLOT_EMPTY,
                                                     std::memory_order_acq_rel)) {
            slots_[i].reset();
            slots_[i].slot_id = i;
            ++count;
        }
    }
    return count;
}

// ── Telemetry ────────────────────────────────────────────────────────────────

uint64_t RingBuffer::active_count() const noexcept {
    uint64_t count = 0;
    for (std::size_t i = 0; i < BUFFER_CAPACITY; ++i) {
        if (slots_[i].state.load(std::memory_order_relaxed) != SLOT_EMPTY)
            ++count;
    }
    return count;
}

double RingBuffer::capacity_percent() const noexcept {
    return (static_cast<double>(active_count()) / BUFFER_CAPACITY) * 100.0;
}

uint64_t RingBuffer::total_enqueued()   const noexcept { return total_enqueued_.load(std::memory_order_relaxed);   }
uint64_t RingBuffer::total_dispatched() const noexcept { return total_dispatched_.load(std::memory_order_relaxed); }

} // namespace nombacrypt
