#include "core/config.hpp"
#include "core/constants.hpp"
#include <cstdlib>
#include <iostream>

namespace nombacrypt {

Config::Config() {
    // Initialization done in load_from_env()
}

Config& Config::get_instance() {
    static Config instance;
    return instance;
}

std::string Config::get_env(const char* key, const std::string& default_value) {
    // Standard C++ getenv works perfectly on MinGW, Linux, and Mac
    const char* val = std::getenv(key);
    if (val) {
        return std::string(val);
    }
    return default_value;
}

int Config::get_env_int(const char* key, int default_value) {
    std::string val = get_env(key);
    if (!val.empty()) {
        try {
            return std::stoi(val);
        } catch (...) {
            std::cerr << "Warning Invalid integer for env var " << key
                      << ". Using default: " << default_value << std::endl;
        }
    }
    return default_value;
}

void Config::load_from_env() {
    // API Credentials
    nomba_client_id    = get_env("NOMBA_TEST_CLIENT_ID");
    nomba_private_key  = get_env("NOMBA_TEST_PRIVATE_KEY");
    nomba_account_id   = get_env("NOMBA_TEST_ACCOUNT_ID");
    nomba_api_base_url = get_env("NOMBA_API_BASE_URL", "https://sandbox.nomba.com/v1");

    // Multi-API Channels (Optional)
    channel_b_client_id   = get_env("NOMBA_CHANNEL_B_CLIENT_ID");
    channel_b_private_key = get_env("NOMBA_CHANNEL_B_PRIVATE_KEY");
    channel_c_client_id   = get_env("NOMBA_CHANNEL_C_CLIENT_ID");
    channel_c_private_key = get_env("NOMBA_CHANNEL_C_PRIVATE_KEY");

    // Engine Configuration
    http_port        = get_env_int("ENGINE_HTTP_PORT", 8080);
    ws_port          = get_env_int("ENGINE_WS_PORT", 8081);
    buffer_capacity  = get_env_int("ENGINE_BUFFER_CAPACITY", BUFFER_CAPACITY);
    ingest_threads   = get_env_int("ENGINE_INGEST_THREADS", DEFAULT_INGEST_THREADS);
    crypto_threads   = get_env_int("ENGINE_CRYPTO_THREADS", DEFAULT_CRYPTO_THREADS);
    dispatch_threads = get_env_int("ENGINE_DISPATCH_THREADS", DEFAULT_DISPATCH_THREADS);
}

} // namespace nombacrypt