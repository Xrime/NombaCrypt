#pragma once

/**
  Environment configuration loader for NombaCrypt Shell.
 */

#include <string>
#include <cstdint>
#include <shared_mutex>

namespace nombacrypt {
    class Config {
    public:
        static Config& get_instance();
        void load_from_env();
        
        // Thread-safe getters
        std::string get_nomba_client_id() const;
        std::string get_nomba_private_key() const;
        std::string get_nomba_account_id() const;
        std::string get_nomba_api_base_url() const;
        
        // Thread-safe setters
        void set_nomba_client_id(const std::string& val);
        void set_nomba_private_key(const std::string& val);
        void set_nomba_account_id(const std::string& val);
        void set_nomba_api_base_url(const std::string& val);

        // Keep these public since they are only read at startup
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

        mutable std::shared_mutex mutex_;
        std::string nomba_client_id_;
        std::string nomba_private_key_;
        std::string nomba_account_id_;
        std::string nomba_api_base_url_;
    };

} // namespace nombacrypt