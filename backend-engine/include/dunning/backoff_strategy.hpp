#pragma once
#include <cstdint>

namespace nombacrypt {

class BackoffStrategy {
public:
    static uint32_t get_next_delay(uint32_t attempt_count);
};

} // namespace nombacrypt
