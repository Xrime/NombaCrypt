#pragma once

#include <string>

namespace nombacrypt {

class HmacVerifier {
public:

    static bool verify_signature(const std::string& payload,
                                 const std::string& signature_hex,
                                 const std::string& secret_key) noexcept;

    static std::string compute_hmac(const std::string& payload,
                                    const std::string& secret_key) noexcept;

    static std::string compute_hmac_hex(const std::string& payload,
                                        const std::string& secret_key) noexcept;

    static std::string sha256_hex(const std::string& message) noexcept;
};

}
