#include "dunning/backoff_strategy.hpp"

namespace nombacrypt {

uint32_t BackoffStrategy::get_next_delay(uint32_t attempt_count) {
    // Exponential backoff logic
    // Attempt 1: 5s, Attempt 2: 15s, Attempt 3: 60s, Attempt >3: 300s
    if (attempt_count <= 1) return 5;
    if (attempt_count == 2) return 15;
    if (attempt_count == 3) return 60;
    return 300;
}

} // namespace nombacrypt