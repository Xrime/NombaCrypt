#pragma once


#include <string>

namespace nombacrypt {


class CardTokenizer {
public:
    CardTokenizer() = delete;  // Pure static utility — no instances

    static std::string tokenize_card(const std::string& card_number,
                                     const std::string& secret_key);

    static bool validate_token(const std::string& card_number,
                               const std::string& token,
                               const std::string& secret_key);

    static std::string masked_last_four(const std::string& card_number);

    static bool luhn_check(const std::string& card_number);

private:
    static constexpr const char* TOKEN_PREFIX = "tok_";

    static bool constant_time_compare(const std::string& a, const std::string& b);
};

}
