#pragma once

#include "core/types.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>

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
    AntiReplayLedger() = default;
    ~AntiReplayLedger() = default;

    mutable std::shared_mutex mutex_;
    std::unordered_map<uint64_t, Timestamp> seen_hashes_; // Hash -> timestamp enqueued in us
};

} // namespace nombacrypt
