
#include "billing/card_tokenizer.hpp"
#include "crypto/hmac_verifier.hpp"
#include "core/logger.hpp"
#include <algorithm>
#include <cctype>

namespace nombacrypt {

std::string CardTokenizer::tokenize_card(const std::string& card_number,
                                         const std::string& secret_key) {
    if (card_number.empty() || secret_key.empty()) {
        LOG_WARN("CardTokenizer::tokenize_card: empty card_number or secret_key");
        return "";
    }

    std::string normalized;
    normalized.reserve(card_number.size());
    for (char c : card_number) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            normalized.push_back(c);
        }
    }

    if (normalized.size() < 8) {
        LOG_WARN("CardTokenizer::tokenize_card: card number too short after normalization");
        return "";
    }


    std::string hmac_hex = HmacVerifier::compute_hmac_hex(normalized, secret_key);

    if (hmac_hex.empty()) {
        LOG_ERROR("CardTokenizer::tokenize_card: HMAC computation failed");
        return "";
    }


    std::string token = std::string(TOKEN_PREFIX) + hmac_hex;

    LOG_INFO("CardTokenizer: tokenized card ending in " << masked_last_four(card_number)
             << " → token length " << token.size());

    return token;
}

bool CardTokenizer::validate_token(const std::string& card_number,
                                   const std::string& token,
                                   const std::string& secret_key) {
    if (card_number.empty() || token.empty() || secret_key.empty()) {
        return false;
    }


    std::string expected_token = tokenize_card(card_number, secret_key);

    if (expected_token.empty()) {
        return false;
    }

    return constant_time_compare(expected_token, token);
}

std::string CardTokenizer::masked_last_four(const std::string& card_number) {

    std::string digits;
    for (char c : card_number) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits.push_back(c);
        }
    }

    if (digits.size() < 4) {
        return "****";
    }

    return digits.substr(digits.size() - 4);
}

bool CardTokenizer::luhn_check(const std::string& card_number) {

    std::string digits;
    for (char c : card_number) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits.push_back(c);
        }
    }

    if (digits.size() < 2) {
        return false;
    }

    int sum = 0;
    bool double_digit = false;


    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        int digit = digits[static_cast<size_t>(i)] - '0';

        if (double_digit) {
            digit *= 2;
            if (digit > 9) {
                digit -= 9;
            }
        }

        sum += digit;
        double_digit = !double_digit;
    }

    return (sum % 10) == 0;
}

bool CardTokenizer::constant_time_compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }

    volatile unsigned char result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);
    }

    return result == 0;
}

}
