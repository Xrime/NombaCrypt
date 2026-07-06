#pragma once
#include <string>
#include <unordered_map>
#include <shared_mutex>

namespace nombacrypt {

class NubanCache {
public:
    static NubanCache& get_instance();

    void cache_account(const std::string& student_id, const std::string& virtual_account);
    std::string get_account(const std::string& student_id);

private:
    NubanCache() = default;
    
    std::unordered_map<std::string, std::string> map_;
    std::shared_mutex mutex_;
};

} // namespace nombacrypt