#pragma once

/**
  Environment configuration loader for NombaCrypt Shell.
 */

#include <string>
#include <cstdint>

namespace nombacrypt {
    class Config {
    public:
        static Config& get_instance();
        void load_from_env();
        std::string nomba_client_id;
        std::string nomba_private_key;
        std::string nomba_account_id;
        std::string nomba_api_base_url;
        std::string channel_b_client_id;
        std::string channel_b_private_key;
        std::string channel_c_client_id;
        std::string channel_c_private_key;
        uint16_t http_port;
        uint16_t ws_port;
        size_t   buffer_capacity;
        size_t   ingest_threads;
        size_t   crypto_threads;
        size_t   dispatch_threads;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;

    private:
        Config();
        std::string get_env(const char* key, const std::string& default_value = "");
        int get_env_int(const char* key, int default_value);
    };

} // namespace nombacrypt