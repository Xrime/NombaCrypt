#pragma once

/**
 * @file config.hpp
 * @brief Runtime configuration for the NombaCrypt Shell backend engine.
 *
 * Reads all tunables from environment variables so that the engine can be
 * configured without recompilation (12-factor style).
 */

#include <cstdint>
#include <string>

namespace nombacrypt {

/**
 * @class Config
 * @brief Singleton-style configuration holder.
 *
 * Populate by calling @ref load_from_env() once at startup.
 */
class Config {
public:
    Config() = default;
    ~Config() = default;

    // ── Loaders ──────────────────────────────────────────────────────
    /**
     * @brief Populate all fields from environment variables.
     * @return true if all mandatory variables were found.
     *
     * Expected env vars:
     *   NOMBA_CLIENT_ID, NOMBA_PRIVATE_KEY,
     *   ENGINE_PORT, WS_PORT, WORKER_COUNT,
     *   LOG_LEVEL, MULTI_API_ENABLED
     */
    bool load_from_env();

    /**
     * @brief Load configuration from a JSON file path.
     * @param filepath Absolute or relative path to a JSON config file.
     * @return true on success.
     */
    bool load_from_file(const std::string& filepath);

    // ── Accessors ────────────────────────────────────────────────────
    [[nodiscard]] const std::string& nomba_client_id()  const noexcept;
    [[nodiscard]] const std::string& nomba_private_key() const noexcept;
    [[nodiscard]] uint16_t           engine_port()       const noexcept;
    [[nodiscard]] uint16_t           ws_port()           const noexcept;
    [[nodiscard]] uint32_t           worker_count()      const noexcept;
    [[nodiscard]] const std::string& log_level()         const noexcept;
    [[nodiscard]] bool               multi_api_enabled() const noexcept;

private:
    std::string nomba_client_id_;
    std::string nomba_private_key_;
    uint16_t    engine_port_       = 9100;
    uint16_t    ws_port_           = 9101;
    uint32_t    worker_count_      = 4;
    std::string log_level_         = "INFO";
    bool        multi_api_enabled_ = false;

    /// Helper: read an env var or return a default.
    static std::string env_or(const char* name, const char* fallback);
};

} // namespace nombacrypt
