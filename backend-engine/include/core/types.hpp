#pragma once

/**
 * @file types.hpp
 * @brief Core type aliases used throughout the NombaCrypt Shell backend engine.
 *
 * Centralises all fundamental type definitions so that every module speaks
 * the same "vocabulary" without pulling in heavyweight headers.
 */

#include <array>
#include <cstdint>
#include <string>
#include <chrono>

namespace nombacrypt {

/// Unique transaction identifier (UUID-v4 string).
using TxId = std::string;

/// Unix-epoch microsecond timestamp.
using Timestamp = uint64_t;

/// Fixed-size buffer for a single webhook / transaction payload.
using PayloadBuffer = std::array<char, 4096>;

/// Monotonic clock used for internal latency measurements.
using SteadyClock = std::chrono::steady_clock;

/// Time-point on the monotonic clock.
using SteadyTimePoint = SteadyClock::time_point;

/// System clock used for wall-clock timestamps (logging, events).
using SystemClock = std::chrono::system_clock;

/// Duration in microseconds (primary latency unit).
using Microseconds = std::chrono::microseconds;

/// Duration in milliseconds (telemetry / timer unit).
using Milliseconds = std::chrono::milliseconds;

/// Hash digest represented as a 64-bit integer.
using PayloadHash = uint64_t;

/// Numeric amount in the smallest currency unit (e.g. kobo).
using AmountMinor = int64_t;

/// User / merchant identifier.
using UserId = std::string;

/// NUBAN (Nigerian Uniform Bank Account Number) — always 10 digits.
using Nuban = std::string;

/// Bearer / OAuth token string.
using AuthToken = std::string;

} // namespace nombacrypt
