#include "crypto/anti_replay_ledger.hpp"
#include <mutex>

namespace nombacrypt {

AntiReplayLedger& AntiReplayLedger::get_instance() {
    static AntiReplayLedger instance;
    return instance;
}

AntiReplayLedger::AntiReplayLedger() {
    eviction_thread_ = std::jthread([this](std::stop_token st) { this->eviction_loop(st); });
}

AntiReplayLedger::~AntiReplayLedger() {
    if (eviction_thread_.joinable()) {
        eviction_thread_.request_stop();
        eviction_thread_.join();
    }
}

bool AntiReplayLedger::insert_and_check(uint64_t hash, Timestamp timestamp_us) noexcept {
    constexpr Timestamp TTL_US = 300ULL * 1000000ULL; // 5 minutes in microseconds
    auto& shard = shards_[hash % NUM_SHARDS];

    std::unique_lock<std::shared_mutex> lock(shard.mutex);

    auto it = shard.hashes.find(hash);
    if (it != shard.hashes.end()) {
        if (timestamp_us - it->second < TTL_US) {
            return false; // Replay attack detected
        }
        it->second = timestamp_us;
        return true;
    }

    shard.hashes[hash] = timestamp_us;
    return true;
}

bool AntiReplayLedger::insert_and_check(uint64_t hash) noexcept {
    return insert_and_check(hash, now_microseconds());
}

void AntiReplayLedger::evict_expired(Timestamp ttl_us) noexcept {
    Timestamp now = now_microseconds();
    
    for (size_t i = 0; i < NUM_SHARDS; ++i) {
        auto& shard = shards_[i];
        std::unique_lock<std::shared_mutex> lock(shard.mutex);

        auto it = shard.hashes.begin();
        while (it != shard.hashes.end()) {
            if (now - it->second >= ttl_us) {
                it = shard.hashes.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void AntiReplayLedger::eviction_loop(std::stop_token st) {
    constexpr Timestamp TTL_US = 300ULL * 1000000ULL; // 5 minutes
    
    while (!st.stop_requested()) {
        evict_expired(TTL_US);
        
        // Sleep for 5 seconds between eviction sweeps, checking for stop requests
        for (int i = 0; i < 50 && !st.stop_requested(); ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void AntiReplayLedger::clear() noexcept {
    for (size_t i = 0; i < NUM_SHARDS; ++i) {
        auto& shard = shards_[i];
        std::unique_lock<std::shared_mutex> lock(shard.mutex);
        shard.hashes.clear();
    }
}

size_t AntiReplayLedger::size() const noexcept {
    size_t total = 0;
    for (size_t i = 0; i < NUM_SHARDS; ++i) {
        auto& shard = shards_[i];
        std::shared_lock<std::shared_mutex> lock(shard.mutex);
        total += shard.hashes.size();
    }
    return total;
}

}
