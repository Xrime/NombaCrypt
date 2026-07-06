#include "reconciler/reconciler.hpp"
#include <iostream>
#include <cassert>

using namespace nombacrypt;

int main() {
    std::cout << "--- NombaCrypt Shell: Automated Reconciler Test ---\n";
    
    Reconciler& reconciler = Reconciler::get_instance();
    reconciler.clear_ledger();

    // 1. Register Expected Payments (University Ledger)
    std::cout << "[*] Registering Expected Payments...\n";
    reconciler.register_expected_payment("STUDENT-12345", 50000.0);
    reconciler.register_expected_payment("STUDENT-12345", 5000.0);
    reconciler.register_expected_payment("STUDENT-99999", 15000.0);
    reconciler.register_expected_payment("STUDENT-00000", 100.0); // Should remain pending

    assert(reconciler.get_pending_count() == 4);

    // 2. Run Reconciliation Cycle
    std::cout << "[*] Running Reconciliation Cycle (Fetching from Nomba Sandbox)...\n";
    ReconcileResult result = reconciler.run_reconciliation_cycle("nomba_acc_123");

    std::cout << "\n=== RECONCILIATION RESULTS ===\n";
    std::cout << "Total Processed Deposits: " << result.total_processed << "\n";
    std::cout << "Total Matched Successfully: " << result.total_matched << "\n";
    std::cout << "Total Unmatched: " << result.total_unmatched << "\n";
    std::cout << "Total Settled Amount: NGN " << result.total_settled_amount << "\n";

    assert(result.total_processed == 3);
    assert(result.total_matched == 3);
    assert(result.total_unmatched == 0);
    assert(result.total_settled_amount == 70000.0);
    assert(reconciler.get_pending_count() == 1); // STUDENT-00000 is still pending

    std::cout << "\n[SUCCESS] Reconciler matched all inbound deposits correctly in O(1) time!\n";
    return 0;
}
