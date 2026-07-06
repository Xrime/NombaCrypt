#include "nuban/nuban_cache.hpp"
#include <mutex>

namespace nombacrypt {

NubanCache& NubanCache::get_instance() {
    static NubanCache instance;
    return instance;
}

void NubanCache::cache_account(const std::string& student_id, const std::string& virtual_account) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    map_[student_id] = virtual_account;
}

std::string NubanCache::get_account(const std::string& student_id) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = map_.find(student_id);
    if (it != map_.end()) {
        return it->second;
    }
    return "";
}

} // namespace nombacrypt