#pragma once


#include <string>
#include <cstdint>
#include "core/types.hpp"

namespace nombacrypt {

enum class InstallmentState : uint8_t {
    ACTIVE    = 0,  ///< Plan is active and accepting payments
    COMPLETED = 1,  ///< All installments have been paid
    DEFAULTED = 2   ///< Plan has defaulted
};

struct InstallmentPlan {
    std::string    plan_id;                ///< Unique plan identifier
    std::string    user_id;               ///< User/customer identifier
    double         total_amount{0.0};     ///< Total amount to be paid
    uint32_t       num_installments{0};   ///< Total number of installments
    double         amount_per_installment{0.0}; ///< Amount per installment
    uint32_t       installments_paid{0};  ///< Number of installments paid so far
    Timestamp      next_due_timestamp{0}; ///< Timestamp of next payment due date
    Timestamp      created_at{0};         ///< When the plan was created
    InstallmentState state{InstallmentState::ACTIVE}; ///< Current plan state

    static constexpr uint64_t DEFAULT_GRACE_PERIOD_US = 30ULL * 24 * 60 * 60 * 1000000ULL;

    static constexpr uint64_t DEFAULT_INSTALLMENT_INTERVAL_US = 30ULL * 24 * 60 * 60 * 1000000ULL;


    double calculate_next_payment() const;

    bool mark_payment_received();

    bool is_overdue() const;

    std::string to_json_string() const;

    static InstallmentPlan create(const std::string& plan_id,
                                  const std::string& user_id,
                                  double total_amount,
                                  uint32_t num_installments,
                                  Timestamp first_due_timestamp = 0);
};
const char* installment_state_to_string(InstallmentState state);

}
