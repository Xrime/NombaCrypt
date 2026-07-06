#include "nuban/nuban_provisioner.hpp"
#include "nuban/nuban_cache.hpp"
#include "core/logger.hpp"
#include "crypto/event_ledger.hpp"
#include <random>
#include <sstream>
#include <iomanip>

namespace nombacrypt {

std::string NubanProvisioner::generate_random_nuban() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1000000000LL, 9999999999LL);
    
    std::ostringstream oss;
    oss << std::setw(10) << std::setfill('0') << dis(gen);
    return oss.str();
}

std::string NubanProvisioner::provision_account(const std::string& student_id) {
    // 1. Check the active memory cache for an existing mapping (O(1) lookup)
    std::string existing = NubanCache::get_instance().get_account(student_id);
    if (!existing.empty()) {
        return existing;
    }

    LOG_INFO("[NubanProvisioner] No account found for " << student_id << ". Provisioning...");

    // 2. Simulate API Call to Nomba Virtual Account API
    // In production, this would be an HTTP POST to api.nomba.com/v1/accounts
    std::string new_nuban = generate_random_nuban();

    // 3. Cache the new mapping into active memory instantly
    NubanCache::get_instance().cache_account(student_id, new_nuban);

    LOG_INFO("[NubanProvisioner] Successfully provisioned NUBAN " << new_nuban << " for " << student_id);
    EventLedger::get_instance().log_event(student_id, "NUBAN_PROVISION", "SUCCESS", "Created account " + new_nuban);

    return new_nuban;
}

} // namespace nombacrypt