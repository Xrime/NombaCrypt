#pragma once

#include <cstdint>

namespace nombacrypt {

struct ReconcileResult {
    uint32_t total_processed;
    uint32_t total_matched;
    uint32_t total_unmatched; // Deposits that didn't match any expected payment
    double total_settled_amount;
};

} // namespace nombacrypt
