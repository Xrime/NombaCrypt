#include "reconciler/reconciler.hpp"
#include <iostream>
#include <mutex>

namespace nombacrypt {

Reconciler& Reconciler::get_instance() {
    static Reconciler instance;
    return instance;
}

void Reconciler::register_expected_payment(const std::string& account_ref, double amount) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    internal_ledger_[account_ref].emplace_back(account_ref, amount);
}

ReconcileResult Reconciler::run_reconciliation_cycle(const std::string& account_id) {
    // 1. Fetch live transactions from Nomba Sandbox API
    std::vector<NombaTransaction> deposits = NombaTransactionsAPI::fetch_recent_deposits(account_id);

    ReconcileResult result{0, 0, 0, 0.0};
    result.total_processed = deposits.size();

    std::unique_lock<std::shared_mutex> lock(mutex_);

    // 2. Dynamically match deposits against the internal ledger
    for (const auto& deposit : deposits) {
        auto it = internal_ledger_.find(deposit.account_reference);
        
        bool matched = false;
        if (it != internal_ledger_.end()) {
            // Find a pending expected payment that matches the amount
            for (auto& expected : it->second) {
                if (expected.status == PaymentStatus::PENDING && 
                    expected.expected_amount == deposit.amount) {
                    
                    // Match found! Settle the payment
                    expected.status = PaymentStatus::SETTLED;
                    result.total_matched++;
                    result.total_settled_amount += deposit.amount;
                    matched = true;
                    break; 
                }
            }
        }
        
        if (!matched) {
            result.total_unmatched++;
        }
    }

    return result;
}

size_t Reconciler::get_pending_count() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& [ref, payments] : internal_ledger_) {
        for (const auto& p : payments) {
            if (p.status == PaymentStatus::PENDING) {
                count++;
            }
        }
    }
    return count;
}

void Reconciler::clear_ledger() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    internal_ledger_.clear();
}

} // namespace nombacrypt
