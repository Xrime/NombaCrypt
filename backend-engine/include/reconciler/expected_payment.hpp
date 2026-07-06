#pragma once

#include "core/types.hpp"
#include <string>
#include <utility>

namespace nombacrypt {

enum class PaymentStatus : uint8_t {
    PENDING = 0,
    SETTLED = 1,
    FAILED = 2
};

struct ExpectedPayment {
    std::string account_reference; // e.g. "STUDENT-12345"
    double expected_amount;
    PaymentStatus status;
    Timestamp created_at_us;
    
    ExpectedPayment() = default;
    
    ExpectedPayment(std::string ref, double amount)
        : account_reference(std::move(ref)), 
          expected_amount(amount), 
          status(PaymentStatus::PENDING),
          created_at_us(now_microseconds()) {}
};

} // namespace nombacrypt
