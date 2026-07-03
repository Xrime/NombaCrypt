#pragma once

/**
 * @file ring_buffer.hpp
 * @brief Lock-free SPMC ring buffer backed by TransactionSlot array.
 *
 * Uses atomic head/tail indices and per-slot state flags to achieve
 * wait-free enqueue and lock-free dequeue without a global mutex.
 * The capacity is fixed at compile-time via @ref BUFFER_CAPACITY.
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "buffer/transaction_slot.hpp"
#include "core/constants.hpp"

namespace nombacrypt {

/**
 * @class RingBuffer
 * @brief Fixed-capacity, cache-line-aligned, lock-free ring buffer.
 */
class RingBuffer {
public:
    RingBuffer();
    ~RingBuffer() = default;

    // Non-copyable / non-movable.
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    /**
     * @brief Enqueue a payload into the next available EMPTY slot.
     * @param data   Pointer to raw payload bytes.
     * @param len    Length of the payload.
     * @param priority  Priority hint (0 = normal).
     * @return The slot index on success, or std::nullopt if the buffer is full.
     */
    std::optional<uint64_t> enqueue(const char* data, uint32_t len, uint8_t priority = 0);

    /**
     * @brief Dequeue the next WRITTEN slot for crypto processing.
     * @return Pointer to the slot (now in PROCESSING state), or nullptr.
     */
    TransactionSlot* dequeue_for_processing();

    /**
     * @brief Mark a PROCESSING slot as DISPATCHED so it can be recycled.
     * @param index  Slot index previously returned by @ref dequeue_for_processing.
     */
    void mark_dispatched(uint64_t index);

    /**
     * @brief Recycle all DISPATCHED slots back to EMPTY.
     * @return Number of slots recycled.
     */
    uint64_t recycle_dispatched();

    // ── Telemetry ────────────────────────────────────────────────────

    /// Number of slots currently not in EMPTY state.
    [[nodiscard]] uint64_t active_count() const noexcept;

    /// Capacity utilisation as a percentage [0, 100].
    [[nodiscard]] double capacity_percent() const noexcept;

    /// Total lifetime enqueue count.
    [[nodiscard]] uint64_t total_enqueued() const noexcept;

    /// Total lifetime dispatch count.
    [[nodiscard]] uint64_t total_dispatched() const noexcept;

    /// Raw capacity.
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return BUFFER_CAPACITY; }

private:
    /// Slot storage, cache-line aligned.
    alignas(64) TransactionSlot slots_[BUFFER_CAPACITY];

    /// Atomic monotonic counters — not masked.  Masking is done on access.
    alignas(64) std::atomic<uint64_t> head_{0}; ///< Next write position.
    alignas(64) std::atomic<uint64_t> tail_{0}; ///< Next read  position.

    /// Lifetime counters (relaxed — telemetry only).
    std::atomic<uint64_t> total_enqueued_{0};
    std::atomic<uint64_t> total_dispatched_{0};

    /// Fast modulo for power-of-two capacity.
    static constexpr uint64_t mask(uint64_t v) noexcept {
        return v & (BUFFER_CAPACITY - 1);
    }
};

} // namespace nombacrypt
