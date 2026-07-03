#pragma once

/**
  Global configuration constants for NombaCrypt Shell.
  Defines hardcoded limits, default thread allocations, and buffer sizes.
 */

#include <cstddef>
#include <cstdint>

namespace nombacrypt {


    /// Must be a power of 2 for fast bitwise modulo operations.
    static constexpr size_t BUFFER_CAPACITY = 8192;

    /// Maximum number of active API channels for the Multi-API dispatcher.
    static constexpr size_t MAX_CHANNELS = 8;

    /// Number of threads parsing HTTP requests and writing to buffer.
    static constexpr size_t DEFAULT_INGEST_THREADS = 2;

    /// Number of threads computing HMAC signatures (Crypto Shield).
    static constexpr size_t DEFAULT_CRYPTO_THREADS = 2;

    /// Number of threads dispatching queued requests to Nomba APIs.
    static constexpr size_t DEFAULT_DISPATCH_THREADS = 3;


    /// Default rate limit: requests per second per channel.
    static constexpr double DEFAULT_RATE_LIMIT_RPS = 100.0;

    /// Maximum number of automated retry attempts for failed card charges.
    static constexpr uint8_t MAX_RETRY_ATTEMPTS = 5;


    /// Interval for aggregating and pushing telemetry data to frontend (ms).
    static constexpr size_t TELEMETRY_INTERVAL_MS = 100;

} // namespace nombacrypt