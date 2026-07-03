#pragma once

/**
 * @file buffer_telemetry.hpp
 * @brief Telemetry snapshot for the ring buffer subsystem.
 *
 * Captured periodically by TelemetryBroadcaster and pushed to the frontend.
 */

#include <cstdint>

namespace nombacrypt {

/**
 * @struct BufferTelemetry
 * @brief Point-in-time statistics for the ring buffer.
 */
struct BufferTelemetry {
    /// Number of slots currently occupied (not EMPTY).
    uint64_t active_count = 0;

    /// Buffer utilisation as a percentage [0.0, 100.0].
    double capacity_percent = 0.0;

    /// Lifetime total of payloads enqueued.
    uint64_t total_enqueued = 0;

    /// Lifetime total of payloads dispatched.
    uint64_t total_dispatched = 0;

    /// Snapshot of slots in WRITTEN state (awaiting crypto).
    uint64_t pending_crypto = 0;

    /// Snapshot of slots in PROCESSING state (awaiting dispatch).
    uint64_t pending_dispatch = 0;
};

} // namespace nombacrypt
