#include "core/config.hpp"
#include "core/constants.hpp"
#include <cstdlib>
#include <iostream>
#include <mutex>

namespace nombacrypt {

Config::Config() {
    // Initialization done in load_from_env()
}

Config& Config::get_instance() {
    static Config instance;
    return instance;
}

std::string Config::get_env(const char* key, const std::string& default_value) {
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
    set_nomba_client_id(get_env("NOMBA_TEST_CLIENT_ID"));
    set_nomba_private_key(get_env("NOMBA_TEST_PRIVATE_KEY"));
    set_nomba_account_id(get_env("NOMBA_TEST_ACCOUNT_ID"));
    set_nomba_api_base_url(get_env("NOMBA_API_BASE_URL", "https://sandbox.nomba.com/v1"));

    // Engine Configuration
    http_port        = get_env_int("ENGINE_HTTP_PORT", 8080);
    ws_port          = get_env_int("ENGINE_WS_PORT", 8081);
    buffer_capacity  = get_env_int("ENGINE_BUFFER_CAPACITY", BUFFER_CAPACITY);
    ingest_threads   = get_env_int("ENGINE_INGEST_THREADS", DEFAULT_INGEST_THREADS);
    crypto_threads   = get_env_int("ENGINE_CRYPTO_THREADS", DEFAULT_CRYPTO_THREADS);
    dispatch_threads = get_env_int("ENGINE_DISPATCH_THREADS", DEFAULT_DISPATCH_THREADS);
}

// ---------------------------------------------------------------------------
// Thread-Safe Getters
// ---------------------------------------------------------------------------
bool Config::get_multi_api_mode() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return multi_api_mode_;
}

std::string Config::get_nomba_client_id() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return nomba_client_id_;
}

std::string Config::get_nomba_private_key() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return nomba_private_key_;
}

std::string Config::get_nomba_account_id() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return nomba_account_id_;
}

std::string Config::get_nomba_api_base_url() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return nomba_api_base_url_;
}

std::string Config::get_channel_b_private_key() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return channel_b_private_key_;
}

std::string Config::get_channel_c_private_key() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return channel_c_private_key_;
}

// ---------------------------------------------------------------------------
// Thread-Safe Setters
// ---------------------------------------------------------------------------
void Config::set_multi_api_mode(bool val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    multi_api_mode_ = val;
}

void Config::set_channel_b_private_key(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    channel_b_private_key_ = val;
}

void Config::set_channel_c_private_key(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    channel_c_private_key_ = val;
}

// ---------------------------------------------------------------------------
void Config::set_nomba_client_id(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    nomba_client_id_ = val;
}

void Config::set_nomba_private_key(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    nomba_private_key_ = val;
}

void Config::set_nomba_account_id(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    nomba_account_id_ = val;
}

void Config::set_nomba_api_base_url(const std::string& val) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    nomba_api_base_url_ = val;
}

} // namespace nombacrypt