#pragma once

#include <string>
#include <vector>

namespace nombacrypt {

struct NombaTransaction {
    std::string transaction_id;
    std::string account_reference; // The NUBAN identifier
    double amount;
    std::string currency;
    std::string time_created;
};

// A mock wrapper for the Nomba Transactions API
class NombaTransactionsAPI {
public:
    // In a real production environment, this would use httplib::Client 
    // to call https://sandbox.nomba.com/v1/transactions
    // For the hackathon demo, we simulate fetching the transactions.
    static std::vector<NombaTransaction> fetch_recent_deposits(const std::string& account_id) {
        // Return a mock payload representing incoming NUBAN transfers
        return {
            {"txn_1001", "STUDENT-12345", 50000.0, "NGN", "2026-07-05T10:00:00Z"},
            {"txn_1002", "STUDENT-99999", 15000.0, "NGN", "2026-07-05T10:05:00Z"},
            {"txn_1003", "STUDENT-12345", 5000.0,  "NGN", "2026-07-05T10:10:00Z"} // A second payment
        };
    }
};

} // namespace nombacrypt
