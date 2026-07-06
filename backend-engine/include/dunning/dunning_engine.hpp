#pragma once
#include "dunning/failed_charge.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace nombacrypt {

class DunningEngine {
public:
    static DunningEngine& get_instance();

    void schedule_retry(const std::string& tx_id, double amount, uint32_t attempt_count);
    void stop();

private:
    DunningEngine();
    ~DunningEngine();
    
    DunningEngine(const DunningEngine&) = delete;
    DunningEngine& operator=(const DunningEngine&) = delete;

    void worker_loop();

    std::priority_queue<FailedCharge> retry_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_thread_;
    std::atomic<bool> running_{true};
};

} // namespace nombacrypt