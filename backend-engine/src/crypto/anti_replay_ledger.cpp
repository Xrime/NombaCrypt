#include "crypto/anti_replay_ledger.hpp"
#include <mutex>

namespace nombacrypt {

AntiReplayLedger& AntiReplayLedger::get_instance() {
    static AntiReplayLedger instance;
    return instance;
}

bool AntiReplayLedger::insert_and_check(uint64_t hash, Timestamp timestamp_us) noexcept {
    constexpr Timestamp TTL_US = 300ULL * 1000000ULL; // 5 minutes in microseconds

    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = seen_hashes_.find(hash);
    if (it != seen_hashes_.end()) {
        if (timestamp_us - it->second < TTL_US) {
            return false; // Replay attack detected
        }
        it->second = timestamp_us;
        return true;
    }

    seen_hashes_[hash] = timestamp_us;
    return true;
}

bool AntiReplayLedger::insert_and_check(uint64_t hash) noexcept {
    return insert_and_check(hash, now_microseconds());
}

void AntiReplayLedger::evict_expired(Timestamp ttl_us) noexcept {
    Timestamp now = now_microseconds();
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = seen_hashes_.begin();
    while (it != seen_hashes_.end()) {
        if (now - it->second >= ttl_us) {
            it = seen_hashes_.erase(it);
        } else {
            ++it;
        }
    }
}

void AntiReplayLedger::clear() noexcept {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    seen_hashes_.clear();
}

size_t AntiReplayLedger::size() const noexcept {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return seen_hashes_.size();
}

}
