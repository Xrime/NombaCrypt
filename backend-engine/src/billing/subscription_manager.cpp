/**
 * @file subscription_manager.cpp
 * @brief Implementation of Toggle #5 — Subscription Matrix.
 *
 * Thread-safe singleton that manages the full lifecycle of
 * recurring billing subscriptions. State machine transitions:
 *   PENDING → PARTIALLY_PAID → SETTLED
 *              └→ CANCELLED (at any point before SETTLED)
 */

#include "billing/subscription_manager.hpp"
#include "core/logger.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nombacrypt {

// ── SubscriptionState helpers ────────────────────────────────────────

const char* subscription_state_to_string(SubscriptionState state) {
    switch (state) {
        case SubscriptionState::PENDING:        return "PENDING";
        case SubscriptionState::PARTIALLY_PAID: return "PARTIALLY_PAID";
        case SubscriptionState::SETTLED:        return "SETTLED";
        case SubscriptionState::CANCELLED:      return "CANCELLED";
        default:                                return "UNKNOWN";
    }
}

// ── Subscription serialization ───────────────────────────────────────

std::string Subscription::to_json_string() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{"
        << "\"subscription_id\":\"" << subscription_id << "\","
        << "\"user_id\":\"" << user_id << "\","
        << "\"total_amount\":" << total_amount << ","
        << "\"installments_total\":" << installments_total << ","
        << "\"installments_paid\":" << installments_paid << ","
        << "\"amount_per_installment\":" << amount_per_installment << ","
        << "\"state\":\"" << subscription_state_to_string(state) << "\","
        << "\"card_token\":\"" << card_token << "\","
        << "\"created_at\":" << created_at << ","
        << "\"updated_at\":" << updated_at << ","
        << "\"plan\":" << plan.to_json_string()
        << "}";
    return oss.str();
}

// ── SubscriptionManager singleton ────────────────────────────────────

SubscriptionManager& SubscriptionManager::get_instance() {
    static SubscriptionManager instance;
    return instance;
}

std::string SubscriptionManager::generate_id() {
    // Simple deterministic ID: "sub_<counter>_<timestamp>"
    std::ostringstream oss;
    oss << "sub_" << next_id_++ << "_" << now_microseconds();
    return oss.str();
}

// ── Create ───────────────────────────────────────────────────────────

std::string SubscriptionManager::create_subscription(const std::string& user_id,
                                                     double total_amount,
                                                     uint32_t installments,
                                                     const std::string& card_token) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_) {
        LOG_WARN("SubscriptionManager: module is disabled (Toggle #5 OFF)");
        return "";
    }

    if (user_id.empty()) {
        LOG_WARN("SubscriptionManager: cannot create subscription — empty user_id");
        return "";
    }

    if (total_amount <= 0.0) {
        LOG_WARN("SubscriptionManager: cannot create subscription — invalid total_amount: "
                 << total_amount);
        return "";
    }

    if (installments == 0) {
        LOG_WARN("SubscriptionManager: cannot create subscription — installments must be > 0");
        return "";
    }

    if (card_token.empty()) {
        LOG_WARN("SubscriptionManager: cannot create subscription — empty card_token");
        return "";
    }

    std::string sub_id = generate_id();
    Timestamp now = now_microseconds();

    Subscription sub;
    sub.subscription_id = sub_id;
    sub.user_id = user_id;
    sub.total_amount = total_amount;
    sub.installments_total = installments;
    sub.installments_paid = 0;
    sub.amount_per_installment = total_amount / static_cast<double>(installments);
    sub.state = SubscriptionState::PENDING;
    sub.card_token = card_token;
    sub.created_at = now;
    sub.updated_at = now;

    // Create the underlying installment plan
    sub.plan = InstallmentPlan::create(sub_id, user_id, total_amount, installments);

    subscriptions_.emplace(sub_id, std::move(sub));

    LOG_INFO("SubscriptionManager: created subscription " << sub_id
             << " for user " << user_id
             << " — " << installments << " installments of "
             << (total_amount / installments)
             << " (total: " << total_amount << ")");

    return sub_id;
}

// ── Process Payment ──────────────────────────────────────────────────

bool SubscriptionManager::process_payment(const std::string& subscription_id,
                                          double amount) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_) {
        LOG_WARN("SubscriptionManager: module is disabled (Toggle #5 OFF)");
        return false;
    }

    auto it = subscriptions_.find(subscription_id);
    if (it == subscriptions_.end()) {
        LOG_WARN("SubscriptionManager: subscription " << subscription_id << " not found");
        return false;
    }

    Subscription& sub = it->second;

    // Can only process payments for PENDING or PARTIALLY_PAID subscriptions
    if (sub.state == SubscriptionState::SETTLED) {
        LOG_WARN("SubscriptionManager: subscription " << subscription_id
                 << " is already SETTLED");
        return false;
    }

    if (sub.state == SubscriptionState::CANCELLED) {
        LOG_WARN("SubscriptionManager: subscription " << subscription_id
                 << " is CANCELLED — cannot process payment");
        return false;
    }

    // Validate payment amount (allow small floating-point tolerance)
    double expected = sub.plan.calculate_next_payment();
    double tolerance = 0.01; // 1 cent tolerance for floating-point math
    if (std::fabs(amount - expected) > tolerance) {
        LOG_WARN("SubscriptionManager: payment amount mismatch for " << subscription_id
                 << " — expected " << expected << ", got " << amount);
        return false;
    }

    // Record the payment on the installment plan
    if (!sub.plan.mark_payment_received()) {
        LOG_ERROR("SubscriptionManager: installment plan rejected payment for "
                  << subscription_id);
        return false;
    }

    sub.installments_paid = sub.plan.installments_paid;
    sub.updated_at = now_microseconds();

    // State transition
    if (sub.plan.state == InstallmentState::COMPLETED) {
        sub.state = SubscriptionState::SETTLED;
        LOG_INFO("SubscriptionManager: subscription " << subscription_id
                 << " → SETTLED (all installments paid)");
    } else if (sub.state == SubscriptionState::PENDING) {
        sub.state = SubscriptionState::PARTIALLY_PAID;
        LOG_INFO("SubscriptionManager: subscription " << subscription_id
                 << " → PARTIALLY_PAID (" << sub.installments_paid
                 << "/" << sub.installments_total << ")");
    } else {
        LOG_INFO("SubscriptionManager: subscription " << subscription_id
                 << " payment " << sub.installments_paid
                 << "/" << sub.installments_total << " processed");
    }

    return true;
}

// ── Query ────────────────────────────────────────────────────────────

bool SubscriptionManager::get_subscription(const std::string& subscription_id,
                                           Subscription& out_sub) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = subscriptions_.find(subscription_id);
    if (it == subscriptions_.end()) {
        return false;
    }

    out_sub = it->second;
    return true;
}

std::vector<Subscription> SubscriptionManager::list_active() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<Subscription> result;
    for (const auto& [id, sub] : subscriptions_) {
        if (sub.state == SubscriptionState::PENDING ||
            sub.state == SubscriptionState::PARTIALLY_PAID) {
            result.push_back(sub);
        }
    }
    return result;
}

std::vector<Subscription> SubscriptionManager::list_by_user(const std::string& user_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<Subscription> result;
    for (const auto& [id, sub] : subscriptions_) {
        if (sub.user_id == user_id) {
            result.push_back(sub);
        }
    }
    return result;
}

// ── Cancel ───────────────────────────────────────────────────────────

bool SubscriptionManager::cancel(const std::string& subscription_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = subscriptions_.find(subscription_id);
    if (it == subscriptions_.end()) {
        LOG_WARN("SubscriptionManager: cannot cancel — subscription "
                 << subscription_id << " not found");
        return false;
    }

    Subscription& sub = it->second;

    if (sub.state == SubscriptionState::SETTLED) {
        LOG_WARN("SubscriptionManager: cannot cancel — subscription "
                 << subscription_id << " is already SETTLED");
        return false;
    }

    if (sub.state == SubscriptionState::CANCELLED) {
        LOG_WARN("SubscriptionManager: subscription "
                 << subscription_id << " is already CANCELLED");
        return false;
    }

    sub.state = SubscriptionState::CANCELLED;
    sub.updated_at = now_microseconds();

    LOG_INFO("SubscriptionManager: subscription " << subscription_id << " → CANCELLED"
             << " (paid " << sub.installments_paid << "/" << sub.installments_total << ")");

    return true;
}

// ── Toggle ───────────────────────────────────────────────────────────

void SubscriptionManager::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
    LOG_INFO("SubscriptionManager: Toggle #5 Subscription Matrix "
             << (enabled ? "ENABLED" : "DISABLED"));
}

bool SubscriptionManager::is_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

size_t SubscriptionManager::count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscriptions_.size();
}

} // namespace nombacrypt
