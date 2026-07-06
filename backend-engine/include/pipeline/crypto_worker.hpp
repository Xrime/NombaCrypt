#pragma once


#include "buffer/ring_buffer.hpp"
#include "crypto/hmac_verifier.hpp"
#include "crypto/sequence_hasher.hpp"
#include "crypto/anti_replay_ledger.hpp"
#include "crypto/event_ledger.hpp"
#include "core/config.hpp"
#include "core/logger.hpp"
#include <atomic>
#include <string>
#include <cstdint>
#include <stop_token>

namespace nombacrypt {

class CryptoWorker {
public:

    explicit CryptoWorker(RingBuffer& buffer);
    void operator()(std::stop_token st);
    void set_enabled(bool enabled) noexcept;
    bool is_enabled() const noexcept;

    /// Telemetry counters
    struct Stats {
        uint64_t total_verified;
        uint64_t total_rejected_signature;
        uint64_t total_rejected_replay;
    };
    Stats get_stats() const noexcept;

private:

    bool verify_slot(TransactionSlot* slot);
    TransactionSlot* find_written_slot();

    RingBuffer& buffer_;
    std::atomic<bool> enabled_{true};

    // Per-worker stats
    alignas(64) std::atomic<uint64_t> stat_verified_{0};
    alignas(64) std::atomic<uint64_t> stat_rejected_sig_{0};
    alignas(64) std::atomic<uint64_t> stat_rejected_replay_{0};
};

}
