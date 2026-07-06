#pragma once
#include <string>
#include <chrono>
#include <cstdint>

namespace nombacrypt {

struct FailedCharge {
    std::string transaction_id;
    double amount;
    uint32_t attempt_count;
    std::chrono::system_clock::time_point next_retry_time;

    // We want the priority queue to act as a min-heap (earliest time first)
    bool operator<(const FailedCharge& other) const {
        return next_retry_time > other.next_retry_time;
    }
};

} // namespace nombacrypt