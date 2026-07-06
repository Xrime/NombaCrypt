/**
 * @file installment_plan.cpp
 * @brief Implementation of the InstallmentPlan data structure.
 */

#include "billing/installment_plan.hpp"
#include "core/logger.hpp"
#include <sstream>
#include <iomanip>

namespace nombacrypt {

const char* installment_state_to_string(InstallmentState state) {
    switch (state) {
        case InstallmentState::ACTIVE:    return "ACTIVE";
        case InstallmentState::COMPLETED: return "COMPLETED";
        case InstallmentState::DEFAULTED: return "DEFAULTED";
        default:                          return "UNKNOWN";
    }
}

double InstallmentPlan::calculate_next_payment() const {
    if (state != InstallmentState::ACTIVE) {
        return 0.0;
    }

    if (installments_paid >= num_installments) {
        return 0.0;
    }

    // For the final installment, pay the remaining balance to handle rounding
    if (installments_paid == num_installments - 1) {
        double paid_so_far = amount_per_installment * installments_paid;
        return total_amount - paid_so_far;
    }

    return amount_per_installment;
}

bool InstallmentPlan::mark_payment_received() {
    if (state != InstallmentState::ACTIVE) {
        LOG_WARN("InstallmentPlan[" << plan_id << "]: cannot record payment — state is "
                 << installment_state_to_string(state));
        return false;
    }

    if (installments_paid >= num_installments) {
        LOG_WARN("InstallmentPlan[" << plan_id << "]: all installments already paid");
        return false;
    }

    ++installments_paid;

    if (installments_paid >= num_installments) {
        state = InstallmentState::COMPLETED;
        next_due_timestamp = 0;
        LOG_INFO("InstallmentPlan[" << plan_id << "]: COMPLETED — all "
                 << num_installments << " installments paid");
    } else {
        // Advance next due date by the standard interval
        next_due_timestamp += DEFAULT_INSTALLMENT_INTERVAL_US;
        LOG_INFO("InstallmentPlan[" << plan_id << "]: payment " << installments_paid
                 << "/" << num_installments << " recorded");
    }

    return true;
}

bool InstallmentPlan::is_overdue() const {
    if (state != InstallmentState::ACTIVE) {
        return false;
    }

    if (next_due_timestamp == 0) {
        return false;
    }

    Timestamp now = now_microseconds();
    return now > (next_due_timestamp + DEFAULT_GRACE_PERIOD_US);
}

std::string InstallmentPlan::to_json_string() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{"
        << "\"plan_id\":\"" << plan_id << "\","
        << "\"user_id\":\"" << user_id << "\","
        << "\"total_amount\":" << total_amount << ","
        << "\"num_installments\":" << num_installments << ","
        << "\"amount_per_installment\":" << amount_per_installment << ","
        << "\"installments_paid\":" << installments_paid << ","
        << "\"next_due_timestamp\":" << next_due_timestamp << ","
        << "\"created_at\":" << created_at << ","
        << "\"state\":\"" << installment_state_to_string(state) << "\""
        << "}";
    return oss.str();
}

InstallmentPlan InstallmentPlan::create(const std::string& plan_id,
                                        const std::string& user_id,
                                        double total_amount,
                                        uint32_t num_installments,
                                        Timestamp first_due_timestamp) {
    InstallmentPlan plan;
    plan.plan_id = plan_id;
    plan.user_id = user_id;
    plan.total_amount = total_amount;
    plan.num_installments = num_installments;

    // Calculate per-installment amount (truncated to 2 decimal places in practice)
    if (num_installments > 0) {
        plan.amount_per_installment = total_amount / static_cast<double>(num_installments);
    }

    plan.installments_paid = 0;
    plan.created_at = now_microseconds();

    // If no first due date specified, default to 30 days from now
    if (first_due_timestamp == 0) {
        plan.next_due_timestamp = plan.created_at + DEFAULT_INSTALLMENT_INTERVAL_US;
    } else {
        plan.next_due_timestamp = first_due_timestamp;
    }

    plan.state = InstallmentState::ACTIVE;

    LOG_INFO("InstallmentPlan[" << plan_id << "]: created for user " << user_id
             << " — " << num_installments << " installments of "
             << plan.amount_per_installment << " (total: " << total_amount << ")");

    return plan;
}

} // namespace nombacrypt
