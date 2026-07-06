#pragma once

#include "reconciler/expected_payment.hpp"
#include "reconciler/reconcile_result.hpp"
#include "nomba/nomba_transactions.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <vector>

namespace nombacrypt {

class Reconciler {
public:
    static Reconciler& get_instance();

    // Disable copy/move
    Reconciler(const Reconciler&) = delete;
    Reconciler& operator=(const Reconciler&) = delete;

    // Add an expected payment to the internal ledger
    void register_expected_payment(const std::string& account_ref, double amount);

    // Fetch Nomba transactions and reconcile them against the ledger
    ReconcileResult run_reconciliation_cycle(const std::string& account_id);

    // Helper for testing
    size_t get_pending_count() const;
    void clear_ledger();

private:
    Reconciler() = default;
    ~Reconciler() = default;

    mutable std::shared_mutex mutex_;
    
    // Internal Ledger: Account Reference -> List of Expected Payments
    // We use a vector because one student might have multiple pending fees
    std::unordered_map<std::string, std::vector<ExpectedPayment>> internal_ledger_;
};

} // namespace nombacrypt
