#pragma once

#include "core/types.hpp"
#include <mutex>

namespace nombacrypt {

class TokenBucketRateLimiter {
public:
    TokenBucketRateLimiter(double tokens_per_second, double burst_capacity);

    bool try_acquire(int tokens = 1);

    double remaining_tokens();


    float headroom_percent();

private:
    void refill(); // Internal helper to calculate time passed and add tokens

    double capacity_;
    double rate_per_sec_;

    double current_tokens_;
    Timestamp last_refill_us_;


    std::mutex mutex_;
};

}