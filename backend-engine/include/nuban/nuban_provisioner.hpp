#pragma once
#include <string>

namespace nombacrypt {

class NubanProvisioner {
public:
    static std::string provision_account(const std::string& student_id);

private:
    static std::string generate_random_nuban();
};

} // namespace nombacrypt