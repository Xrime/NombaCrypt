/**
 * @file config.cpp
 * @brief Implementation of nombacrypt::Config.
 */

#include "core/config.hpp"

#include <cstdlib>

namespace nombacrypt {

// ── Helpers ──────────────────────────────────────────────────────────────────

std::string Config::env_or(const char* name, const char* fallback) {
    const char* val = std::getenv(name);
    return val ? std::string(val) : std::string(fallback);
}

// ── Loaders ──────────────────────────────────────────────────────────────────

bool Config::load_from_env() {
    nomba_client_id_   = env_or("NOMBA_CLIENT_ID", "");
    nomba_private_key_ = env_or("NOMBA_PRIVATE_KEY", "");
    engine_port_       = static_cast<uint16_t>(std::stoi(env_or("ENGINE_PORT", "9100")));
    ws_port_           = static_cast<uint16_t>(std::stoi(env_or("WS_PORT", "9101")));
    worker_count_      = static_cast<uint32_t>(std::stoul(env_or("WORKER_COUNT", "4")));
    log_level_         = env_or("LOG_LEVEL", "INFO");
    multi_api_enabled_ = env_or("MULTI_API_ENABLED", "0") == "1";

    // TODO: validate mandatory fields and return false if missing
    return !nomba_client_id_.empty() && !nomba_private_key_.empty();
}

bool Config::load_from_file(const std::string& /*filepath*/) {
    // TODO: parse JSON config file and populate fields
    return false;
}

// ── Accessors ────────────────────────────────────────────────────────────────

const std::string& Config::nomba_client_id()  const noexcept { return nomba_client_id_;   }
const std::string& Config::nomba_private_key() const noexcept { return nomba_private_key_; }
uint16_t           Config::engine_port()       const noexcept { return engine_port_;       }
uint16_t           Config::ws_port()           const noexcept { return ws_port_;           }
uint32_t           Config::worker_count()      const noexcept { return worker_count_;      }
const std::string& Config::log_level()         const noexcept { return log_level_;         }
bool               Config::multi_api_enabled() const noexcept { return multi_api_enabled_; }

} // namespace nombacrypt
