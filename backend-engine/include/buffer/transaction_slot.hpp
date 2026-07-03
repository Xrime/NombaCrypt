#pragma once

/**
 * @file transaction_slot.hpp
 * @brief A single fixed-size slot inside the lock-free ring buffer.
 *
 * Each slot holds one raw transaction payload and metadata.  The `state`
 * field is an atomic that drives the 4-state life-cycle:
 *   EMPTY → WRITTEN → PROCESSING → DISPATCHED (→ EMPTY on recycle).
 */

#include <atomic>
#include <cstdint>
#include <cstring>

#include "core/constants.hpp"
#include "core/types.hpp"

namespace nombacrypt {

/// Slot state values (uint8_t so the atomic is lock-free everywhere).
enum SlotState : uint8_t {
    SLOT_EMPTY      = 0, ///< Available for writing.
    SLOT_WRITTEN    = 1, ///< Payload written, awaiting crypto verification.
    SLOT_PROCESSING = 2, ///< Crypto verified, awaiting dispatch.
    SLOT_DISPATCHED = 3, ///< Dispatched — can be recycled.
};

/**
 * @struct TransactionSlot
 * @brief Cache-line-aligned slot for one transaction payload.
 */
struct alignas(64) TransactionSlot {
    /// Monotonically increasing slot identifier within the ring.
    uint64_t slot_id = 0;

    /// Raw payload bytes.
    char payload[MAX_PAYLOAD_SIZE]{};

    /// Actual number of valid bytes in @ref payload.
    uint32_t payload_len = 0;

    /// Monotonic microsecond timestamp when the payload was ingested.
    uint64_t ingested_at_us = 0;

    /// Priority hint (0 = normal, higher = more urgent).
    uint8_t priority = 0;

    /// Atomic state flag — drives the slot life-cycle.
    std::atomic<uint8_t> state{SLOT_EMPTY};

    // ── Convenience helpers ──────────────────────────────────────────

    /// Reset the slot so it can be reused.
    void reset() noexcept {
        slot_id        = 0;
        payload_len    = 0;
        ingested_at_us = 0;
        priority       = 0;
        state.store(SLOT_EMPTY, std::memory_order_release);
    }

    /// Write payload data into the slot (caller must guarantee EMPTY state).
    void write(const char* data, uint32_t len, uint8_t prio = 0) noexcept {
        payload_len = (len <= MAX_PAYLOAD_SIZE) ? len : static_cast<uint32_t>(MAX_PAYLOAD_SIZE);
        std::memcpy(payload, data, payload_len);
        priority = prio;
        // TODO: set ingested_at_us from SteadyClock
    }
};

} // namespace nombacrypt
