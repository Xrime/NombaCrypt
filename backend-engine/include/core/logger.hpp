#pragma once

/**
 * @file logger.hpp
 * @brief Thread-safe logging facility for the NombaCrypt Shell backend engine.
 *
 * Provides LOG_INFO, LOG_WARN, LOG_ERROR convenience macros that capture
 * file/line information and delegate to Logger::log().
 */

#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>

namespace nombacrypt {

/// Severity levels.
enum class LogLevel : uint8_t {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    ERROR = 3,
    FATAL = 4,
};

/**
 * @class Logger
 * @brief Singleton thread-safe logger writing to stdout.
 *
 * Output format:
 *   [2026-07-03T18:00:00.123Z] [tid:12345] [INFO ] module: message
 */
class Logger {
public:
    /// Obtain the global logger instance.
    static Logger& instance();

    /// Set the minimum severity that will be emitted.
    void set_level(LogLevel level) noexcept;

    /// Core logging method — prefer the macros below.
    void log(LogLevel level,
             std::string_view module,
             std::string_view message,
             const char* file = __builtin_FILE(),
             int line = __builtin_LINE());

    /// Convenience typed wrappers.
    void info (std::string_view module, std::string_view message);
    void warn (std::string_view module, std::string_view message);
    void error(std::string_view module, std::string_view message);
    void debug(std::string_view module, std::string_view message);
    void fatal(std::string_view module, std::string_view message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::mutex mutex_;
    LogLevel   min_level_ = LogLevel::INFO;

    /// Format the current wall-clock time as ISO-8601.
    static std::string timestamp_now();

    /// Return the current thread ID as a string.
    static std::string thread_id_str();
};

} // namespace nombacrypt

// ── Convenience Macros ───────────────────────────────────────────────────────

/// @def LOG_INFO(module, msg)
#define LOG_INFO(module, msg)  ::nombacrypt::Logger::instance().info(module, msg)

/// @def LOG_WARN(module, msg)
#define LOG_WARN(module, msg)  ::nombacrypt::Logger::instance().warn(module, msg)

/// @def LOG_ERROR(module, msg)
#define LOG_ERROR(module, msg) ::nombacrypt::Logger::instance().error(module, msg)

/// @def LOG_DEBUG(module, msg)
#define LOG_DEBUG(module, msg) ::nombacrypt::Logger::instance().debug(module, msg)
