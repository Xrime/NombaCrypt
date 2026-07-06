#pragma once

#include "core/types.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>
#include <thread>
#include <stop_token>

namespace nombacrypt {

class AntiReplayLedger {
public:
    
    static AntiReplayLedger& get_instance();

    // Disable copy/move constructors and assignment operators
    AntiReplayLedger(const AntiReplayLedger&) = delete;
    AntiReplayLedger& operator=(const AntiReplayLedger&) = delete;
    AntiReplayLedger(AntiReplayLedger&&) = delete;
    AntiReplayLedger& operator=(AntiReplayLedger&&) = delete;

    
    bool insert_and_check(uint64_t hash, Timestamp timestamp_us) noexcept;

    
    bool insert_and_check(uint64_t hash) noexcept;

    
    void evict_expired(Timestamp ttl_us) noexcept;

    
    void clear() noexcept;

    
    size_t size() const noexcept;

private:
    AntiReplayLedger();
    ~AntiReplayLedger();
    void eviction_loop(std::stop_token st);

    static constexpr size_t NUM_SHARDS = 64;
    struct Shard {
        mutable std::shared_mutex mutex;
        std::unordered_map<uint64_t, Timestamp> hashes;
    };
    
    Shard shards_[NUM_SHARDS];
    std::jthread eviction_thread_;
};

} // namespace nombacrypt
