#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include "core/types.hpp"
#include "billing/installment_plan.hpp"

namespace nombacrypt {

enum class SubscriptionState : uint8_t {
    PENDING        = 0,  ///< Subscription created, no payments yet
    PARTIALLY_PAID = 1,  ///< Some installments paid, more remaining
    SETTLED        = 2,  ///< All installments paid in full
    CANCELLED      = 3   ///< Subscription was cancelled
};
const char* subscription_state_to_string(SubscriptionState state);

struct Subscription {
    std::string       subscription_id;        ///< Unique subscription identifier
    std::string       user_id;                ///< User/customer identifier
    double            total_amount{0.0};      ///< Total subscription cost
    uint32_t          installments_total{0};  ///< Total number of installments
    uint32_t          installments_paid{0};   ///< Number paid so far
    double            amount_per_installment{0.0}; ///< Per-installment amount
    SubscriptionState state{SubscriptionState::PENDING}; ///< Current state
    std::string       card_token;             ///< Tokenized card for auto-charge
    Timestamp         created_at{0};          ///< Creation timestamp
    Timestamp         updated_at{0};          ///< Last update timestamp
    InstallmentPlan   plan;                   ///< Underlying installment plan

    std::string to_json_string() const;
};

class SubscriptionManager {
public:
    static SubscriptionManager& get_instance();

    SubscriptionManager(const SubscriptionManager&) = delete;
    SubscriptionManager& operator=(const SubscriptionManager&) = delete;
    std::string create_subscription(const std::string& user_id,
                                    double total_amount,
                                    uint32_t installments,
                                    const std::string& card_token);
    bool process_payment(const std::string& subscription_id, double amount);


    bool get_subscription(const std::string& subscription_id, Subscription& out_sub) const;

    std::vector<Subscription> list_active() const;

    std::vector<Subscription> list_by_user(const std::string& user_id) const;

    bool cancel(const std::string& subscription_id);

    void set_enabled(bool enabled);

    bool is_enabled() const;

    size_t count() const;

private:
    SubscriptionManager() = default;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Subscription> subscriptions_;
    bool enabled_{false};
    uint64_t next_id_{1};  ///< Auto-incrementing ID for subscription generation

    std::string generate_id();
};

}
