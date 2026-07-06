#include "dunning/dunning_engine.hpp"
#include "dunning/backoff_strategy.hpp"
#include "core/logger.hpp"
#include "crypto/event_ledger.hpp"

namespace nombacrypt {

DunningEngine& DunningEngine::get_instance() {
    static DunningEngine instance;
    return instance;
}

DunningEngine::DunningEngine() {
    worker_thread_ = std::thread(&DunningEngine::worker_loop, this);
}

DunningEngine::~DunningEngine() {
    stop();
}

void DunningEngine::stop() {
    if (running_.exchange(false)) {
        cv_.notify_all();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }
}

void DunningEngine::schedule_retry(const std::string& tx_id, double amount, uint32_t attempt_count) {
    uint32_t delay_secs = BackoffStrategy::get_next_delay(attempt_count);
    auto next_time = std::chrono::system_clock::now() + std::chrono::seconds(delay_secs);

    FailedCharge charge{tx_id, amount, attempt_count, next_time};

    {
        std::lock_guard<std::mutex> lock(mutex_);
        retry_queue_.push(charge);
    }
    cv_.notify_one();

    LOG_INFO("[DunningEngine] Scheduled retry for tx " << tx_id << " in " << delay_secs << "s (Attempt " << attempt_count << ")");
}

void DunningEngine::worker_loop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (retry_queue_.empty()) {
            cv_.wait(lock, [this] { return !running_ || !retry_queue_.empty(); });
        } else {
            auto now = std::chrono::system_clock::now();
            auto top = retry_queue_.top();

            if (now >= top.next_retry_time) {
                retry_queue_.pop();
                lock.unlock(); // Unlock while processing the external HTTP request simulation

                LOG_INFO("[DunningEngine] Executing retry for tx " << top.transaction_id << " (Attempt " << top.attempt_count << ")");

                // Simulate processing the retry via Nomba API
                // For hackathon, we assume 80% success rate on retry
                bool success = (top.attempt_count >= 2) ? true : false; 

                if (success) {
                    LOG_INFO("[DunningEngine] Retry SUCCESS for tx " << top.transaction_id);
                    EventLedger::get_instance().log_event(top.transaction_id, "DUNNING_RETRY", "SUCCESS", "Recovered failed payment");
                } else {
                    LOG_WARN("[DunningEngine] Retry FAILED for tx " << top.transaction_id);
                    EventLedger::get_instance().log_event(top.transaction_id, "DUNNING_RETRY", "FAILED", "Attempt " + std::to_string(top.attempt_count) + " failed");
                    // Re-schedule for next attempt
                    schedule_retry(top.transaction_id, top.amount, top.attempt_count + 1);
                }

            } else {
                // Wait until the next retry time or until a new (earlier) item is pushed
                cv_.wait_until(lock, top.next_retry_time);
            }
        }
    }
}

} // namespace nombacrypt