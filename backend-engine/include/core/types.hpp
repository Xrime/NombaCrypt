#pragma once
/**
  Core type aliases used across the entire NombaCrypt Shell engine.
  Centralizes type definitions to ensure consistency between modules.
  All modules should include this header instead of defining their own
  primitive types.
 */

#include <cstdint>
#include <string>
#include <array>
#include <chrono>

namespace nombacrypt {

/// Unique transaction identifier (UUIDv4 string)
using TxId = std::string;

/// Monotonic slot sequence number
using SlotIndex = uint64_t;

/// Microsecond-precision timestamp (Unix epoch)
using Timestamp = uint64_t;

/// High-resolution clock for latency measurement
using HiResClock = std::chrono::high_resolution_clock;
using TimePoint  = std::chrono::time_point<HiResClock>;

/// Maximum size of a single transaction payload in bytes
static constexpr size_t MAX_PAYLOAD_SIZE = 4096;

/// Fixed-size buffer for a single transaction payload
using PayloadBuffer = std::array<char, MAX_PAYLOAD_SIZE>;

enum class SlotState : uint8_t {
    EMPTY      = 0,  ///< Slot is free and available for writing
    WRITTEN    = 1,  ///< Payload ingested, awaiting crypto verification
    PROCESSING = 2,  ///< Crypto verified, awaiting dispatch to Nomba API
    DISPATCHED = 3   ///< Successfully dispatched, ready for recycling
};

/**
 * @enum BufferStatus
 * @brief Transaction queue tracking state sent to the frontend.
 */
enum class BufferStatus : uint8_t {
    QUEUED     = 0,
    PROCESSING = 1,
    SETTLED    = 2,
    FAILED     = 3
};

inline Timestamp now_microseconds() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

} // namespace nombacrypt