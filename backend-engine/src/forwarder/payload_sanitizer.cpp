/**
 * @file payload_sanitizer.cpp
 * @brief Implementation of security input validation and sanitization.
 *
 * All operations use raw string manipulation since no JSON library is
 * available. The sanitizer is intentionally conservative — it rejects
 * payloads that look suspicious rather than trying to fix them.
 */

#include "forwarder/payload_sanitizer.hpp"
#include "core/logger.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace nombacrypt {

// ---------------------------------------------------------------------------
// Private helper: case-insensitive substring search
// ---------------------------------------------------------------------------
bool PayloadSanitizer::icontains(const std::string& haystack,
                                  const std::string& needle) {
    if (needle.empty() || needle.size() > haystack.size()) return false;

    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) ==
                   std::tolower(static_cast<unsigned char>(b));
        });
    return it != haystack.end();
}

// ---------------------------------------------------------------------------
// Private helper: strip non-printable control characters (keep whitespace)
// ---------------------------------------------------------------------------
std::string PayloadSanitizer::strip_control_chars(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (char c : input) {
        auto uc = static_cast<unsigned char>(c);
        // Keep printable characters and standard whitespace (space, tab, CR, LF)
        if (uc >= 0x20 || uc == '\t' || uc == '\n' || uc == '\r') {
            result += c;
        }
        // Silently drop other control characters (0x00-0x1F except above)
    }
    return result;
}

// ---------------------------------------------------------------------------
// Private: list of sensitive JSON field names to redact
// ---------------------------------------------------------------------------
const std::vector<std::string>& PayloadSanitizer::sensitive_fields() {
    static const std::vector<std::string> fields = {
        "card_number",
        "card_num",
        "pan",
        "cvv",
        "cvc",
        "password",
        "passwd",
        "pin",
        "secret",
        "secret_key",
        "token",
        "access_token",
        "refresh_token",
        "authorization",
        "auth_token",
        "api_key",
        "private_key",
        "ssn",
        "account_number"
    };
    return fields;
}

// ---------------------------------------------------------------------------
// Private: SQL injection patterns (case-insensitive matching)
// ---------------------------------------------------------------------------
const std::vector<std::string>& PayloadSanitizer::sql_patterns() {
    static const std::vector<std::string> patterns = {
        "' or ",
        "' and ",
        "'; drop ",
        "'; delete ",
        "'; insert ",
        "'; update ",
        "'; select ",
        "union select",
        "union all select",
        "1=1",
        "1' or '1'='1",
        "' or 1=1",
        "'; exec ",
        "'; execute ",
        "--",
        "/*",
        "xp_cmdshell",
        "sp_executesql",
        "char(0x",
        "cast(0x",
        "convert(int",
        "waitfor delay",
        "benchmark(",
        "sleep("
    };
    return patterns;
}

// ---------------------------------------------------------------------------
// Private: XSS injection patterns (case-insensitive matching)
// ---------------------------------------------------------------------------
const std::vector<std::string>& PayloadSanitizer::xss_patterns() {
    static const std::vector<std::string> patterns = {
        "<script",
        "</script",
        "javascript:",
        "onerror=",
        "onload=",
        "onclick=",
        "onmouseover=",
        "onfocus=",
        "onblur=",
        "<iframe",
        "<object",
        "<embed",
        "<svg",
        "eval(",
        "document.cookie",
        "document.write",
        "window.location",
        "alert(",
        "String.fromCharCode",
        "atob(",
        "btoa("
    };
    return patterns;
}

// ---------------------------------------------------------------------------
// Public: contains_injection — check for SQL injection and XSS patterns
// ---------------------------------------------------------------------------
bool PayloadSanitizer::contains_injection(const std::string& input) {
    if (input.empty()) return false;

    for (const auto& pattern : sql_patterns()) {
        if (icontains(input, pattern)) {
            LOG_WARN("[PayloadSanitizer] SQL injection pattern detected: "
                     << pattern);
            return true;
        }
    }

    for (const auto& pattern : xss_patterns()) {
        if (icontains(input, pattern)) {
            LOG_WARN("[PayloadSanitizer] XSS injection pattern detected: "
                     << pattern);
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// Private: redact the value of a specific JSON key
//
// Handles the pattern:  "field_name" : "some_value"
// Replaces with:        "field_name" : "[REDACTED]"
//
// Also handles numeric values:  "field_name" : 12345
// Replaces with:                "field_name" : "[REDACTED]"
// ---------------------------------------------------------------------------
std::string PayloadSanitizer::redact_field_value(const std::string& json,
                                                  const std::string& field_name) {
    std::string result = json;
    // Build the search key: "field_name"
    std::string search_key = "\"" + field_name + "\"";

    size_t pos = 0;
    while (true) {
        // Find the key (case-insensitive search on the key portion)
        size_t key_pos = std::string::npos;
        for (size_t i = pos; i + search_key.size() <= result.size(); ++i) {
            bool match = true;
            for (size_t j = 0; j < search_key.size(); ++j) {
                if (std::tolower(static_cast<unsigned char>(result[i + j])) !=
                    std::tolower(static_cast<unsigned char>(search_key[j]))) {
                    match = false;
                    break;
                }
            }
            if (match) {
                key_pos = i;
                break;
            }
        }

        if (key_pos == std::string::npos) break;

        // Find the colon after the key
        size_t colon_pos = result.find(':', key_pos + search_key.size());
        if (colon_pos == std::string::npos) break;

        // Skip whitespace after colon
        size_t value_start = colon_pos + 1;
        while (value_start < result.size() &&
               std::isspace(static_cast<unsigned char>(result[value_start]))) {
            ++value_start;
        }

        if (value_start >= result.size()) break;

        size_t value_end = value_start;

        if (result[value_start] == '"') {
            // String value: find the closing quote (handle escaped quotes)
            value_end = value_start + 1;
            while (value_end < result.size()) {
                if (result[value_end] == '\\') {
                    value_end += 2; // skip escaped character
                    continue;
                }
                if (result[value_end] == '"') {
                    value_end++; // include closing quote
                    break;
                }
                value_end++;
            }
        } else {
            // Numeric or other value: read until comma, }, or whitespace
            while (value_end < result.size() &&
                   result[value_end] != ',' &&
                   result[value_end] != '}' &&
                   result[value_end] != ']' &&
                   !std::isspace(static_cast<unsigned char>(result[value_end]))) {
                value_end++;
            }
        }

        // Replace the value with "[REDACTED]"
        std::string redacted = "\"[REDACTED]\"";
        result = result.substr(0, value_start) + redacted +
                 result.substr(value_end);

        // Advance past the replacement to avoid infinite loop
        pos = value_start + redacted.size();
    }

    return result;
}

// ---------------------------------------------------------------------------
// Public: validate_payload
// ---------------------------------------------------------------------------
bool PayloadSanitizer::validate_payload(const std::string& json) {
    // Check: non-empty
    if (json.empty()) {
        LOG_WARN("[PayloadSanitizer] Validation failed: empty payload");
        return false;
    }

    // Check: max length
    if (json.size() > MAX_PAYLOAD_LENGTH) {
        LOG_WARN("[PayloadSanitizer] Validation failed: payload too large ("
                 << json.size() << " bytes, max " << MAX_PAYLOAD_LENGTH << ")");
        return false;
    }

    // Check: looks like JSON (starts with { or [)
    size_t first_non_ws = 0;
    while (first_non_ws < json.size() &&
           std::isspace(static_cast<unsigned char>(json[first_non_ws]))) {
        ++first_non_ws;
    }
    if (first_non_ws >= json.size() ||
        (json[first_non_ws] != '{' && json[first_non_ws] != '[')) {
        LOG_WARN("[PayloadSanitizer] Validation failed: does not look like JSON");
        return false;
    }

    // Check: contains required fields (transaction_id and amount)
    if (!icontains(json, "\"transaction_id\"")) {
        LOG_WARN("[PayloadSanitizer] Validation failed: missing 'transaction_id'");
        return false;
    }
    if (!icontains(json, "\"amount\"")) {
        LOG_WARN("[PayloadSanitizer] Validation failed: missing 'amount'");
        return false;
    }

    // Check: no injection patterns
    if (contains_injection(json)) {
        LOG_WARN("[PayloadSanitizer] Validation failed: injection pattern found");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Public: strip_sensitive_fields
// ---------------------------------------------------------------------------
std::string PayloadSanitizer::strip_sensitive_fields(const std::string& json) {
    std::string result = json;
    for (const auto& field : sensitive_fields()) {
        result = redact_field_value(result, field);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Public: sanitize — full pipeline
// ---------------------------------------------------------------------------
std::string PayloadSanitizer::sanitize(const std::string& raw_json) {
    // Step 1: Strip control characters
    std::string cleaned = strip_control_chars(raw_json);

    // Step 2: Trim leading/trailing whitespace
    {
        size_t start = 0;
        while (start < cleaned.size() &&
               std::isspace(static_cast<unsigned char>(cleaned[start]))) {
            ++start;
        }
        size_t end = cleaned.size();
        while (end > start &&
               std::isspace(static_cast<unsigned char>(cleaned[end - 1]))) {
            --end;
        }
        cleaned = cleaned.substr(start, end - start);
    }

    // Step 3: Validate the cleaned payload
    if (!validate_payload(cleaned)) {
        LOG_WARN("[PayloadSanitizer] Sanitization failed: payload rejected");
        return "";
    }

    // Step 4: Strip sensitive fields
    cleaned = strip_sensitive_fields(cleaned);

    LOG_INFO("[PayloadSanitizer] Payload sanitized successfully ("
             << cleaned.size() << " bytes)");

    return cleaned;
}

} // namespace nombacrypt
