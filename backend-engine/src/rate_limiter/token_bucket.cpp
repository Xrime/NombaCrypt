#include "../../include/rate_limiter/token_bucket.hpp"
#include <algorithm>

namespace nombacrypt {

    TokenBucketRateLimiter::TokenBucketRateLimiter(double tokens_per_second, double burst_capacity)
        : capacity_(burst_capacity),
          rate_per_sec_(tokens_per_second),
          current_tokens_(burst_capacity), // Start with a full bucket
          last_refill_us_(now_microseconds()) {}

    void TokenBucketRateLimiter::refill() {
        Timestamp now = now_microseconds();
        double elapsed_sec = static_cast<double>(now - last_refill_us_) / 1000000.0;

        // Add tokens based on how much time has passed
        current_tokens_ += elapsed_sec * rate_per_sec_;

        // Cap it at maximum capacity so it doesn't overflow
        if (current_tokens_ > capacity_) {
            current_tokens_ = capacity_;
        }

        last_refill_us_ = now;
    }

    bool TokenBucketRateLimiter::try_acquire(int tokens) {
        std::lock_guard<std::mutex> lock(mutex_); // Thread safety
        refill(); // Update token count before checking

        if (current_tokens_ >= tokens) {
            current_tokens_ -= tokens;
            return true; // Approved
        }
        return false; // Rejected (Rate Limited)
    }

    double TokenBucketRateLimiter::remaining_tokens() {
        std::lock_guard<std::mutex> lock(mutex_);
        refill();
        return current_tokens_;
    }

    float TokenBucketRateLimiter::headroom_percent() {
        std::lock_guard<std::mutex> lock(mutex_);
        refill();
        return static_cast<float>((current_tokens_ / capacity_) * 100.0);
    }

} // namespace nombacrypt