#pragma once

/**
 * @file constants.hpp
 * @brief Global compile-time constants for the NombaCrypt Shell backend engine.
 *
 * All tunables that affect buffer capacity, retry logic, telemetry cadence,
 * and channel limits are defined here so they can be adjusted in one place.
 */

#include <cstddef>
#include <cstdint>

namespace nombacrypt {

/// Ring-buffer slot count (must be a power of two for modulo tricks).
inline constexpr std::size_t BUFFER_CAPACITY = 8192;

/// Maximum raw payload size in bytes that a single transaction slot can hold.
inline constexpr std::size_t MAX_PAYLOAD_SIZE = 4096;

/// Maximum number of simultaneous Nomba API channels.
inline constexpr std::size_t MAX_CHANNELS = 8;

/// How often (ms) the telemetry broadcaster pushes stats to the frontend.
inline constexpr uint32_t TELEMETRY_INTERVAL_MS = 100;

/// Maximum number of retry attempts before a transaction is dropped.
inline constexpr uint32_t MAX_RETRY_ATTEMPTS = 5;

/// Default HTTP server listen port.
inline constexpr uint16_t DEFAULT_ENGINE_PORT = 9100;

/// Default WebSocket server port for IPC telemetry.
inline constexpr uint16_t DEFAULT_WS_PORT = 9101;

/// Anti-replay hash TTL in seconds.
inline constexpr uint32_t ANTI_REPLAY_TTL_SECONDS = 300;

/// Token-bucket default refill rate (tokens per second).
inline constexpr double DEFAULT_REFILL_RATE = 50.0;

/// Token-bucket default burst capacity.
inline constexpr uint32_t DEFAULT_BURST_CAPACITY = 200;

/// Circuit-breaker: consecutive failures before marking channel DOWN.
inline constexpr uint32_t CIRCUIT_BREAKER_THRESHOLD = 5;

/// Circuit-breaker: cooldown period in milliseconds.
inline constexpr uint32_t CIRCUIT_BREAKER_COOLDOWN_MS = 30'000;

/// Worker pool default thread count (per stage).
inline constexpr uint32_t DEFAULT_WORKER_COUNT = 4;

/// Dunning: base delay for exponential backoff (seconds).
inline constexpr uint32_t DUNNING_BASE_DELAY_S = 60;

/// Dunning: maximum delay cap (seconds).
inline constexpr uint32_t DUNNING_MAX_DELAY_S = 86'400; // 24 h

} // namespace nombacrypt
