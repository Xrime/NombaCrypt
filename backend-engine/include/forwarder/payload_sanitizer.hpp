#pragma once


#include <string>
#include <vector>

namespace nombacrypt {

class PayloadSanitizer {
public:
    PayloadSanitizer() = delete;

    static std::string sanitize(const std::string& raw_json);

    static bool validate_payload(const std::string& json);

    static std::string strip_sensitive_fields(const std::string& json);

    static bool contains_injection(const std::string& input);

private:
    static constexpr size_t MAX_PAYLOAD_LENGTH = 4096;

    static const std::vector<std::string>& sensitive_fields();

    static const std::vector<std::string>& sql_patterns();

    static const std::vector<std::string>& xss_patterns();


    static bool icontains(const std::string& haystack,
                          const std::string& needle);

    static std::string redact_field_value(const std::string& json,
                                          const std::string& field_name);

    static std::string strip_control_chars(const std::string& input);
};

}
