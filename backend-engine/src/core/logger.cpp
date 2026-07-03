/**
 * @file logger.cpp
 * @brief Implementation of nombacrypt::Logger.
 */

#include "core/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace nombacrypt {

// ── Singleton ────────────────────────────────────────────────────────────────

Logger& Logger::instance() {
    static Logger singleton;
    return singleton;
}

// ── Configuration ────────────────────────────────────────────────────────────

void Logger::set_level(LogLevel level) noexcept {
    min_level_ = level;
}

// ── Core log method ──────────────────────────────────────────────────────────

void Logger::log(LogLevel level,
                 std::string_view module,
                 std::string_view message,
                 const char* /*file*/,
                 int /*line*/) {
    if (level < min_level_) return;

    static constexpr const char* LABELS[] = {
        "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"
    };

    const auto idx = static_cast<uint8_t>(level);
    const char* label = (idx < 5) ? LABELS[idx] : "?????";

    std::ostringstream oss;
    oss << "[" << timestamp_now() << "] "
        << "[tid:" << thread_id_str() << "] "
        << "[" << label << "] "
        << module << ": " << message << "\n";

    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << oss.str() << std::flush;
    }
}

// ── Convenience wrappers ─────────────────────────────────────────────────────

void Logger::info (std::string_view m, std::string_view msg) { log(LogLevel::INFO,  m, msg); }
void Logger::warn (std::string_view m, std::string_view msg) { log(LogLevel::WARN,  m, msg); }
void Logger::error(std::string_view m, std::string_view msg) { log(LogLevel::ERROR, m, msg); }
void Logger::debug(std::string_view m, std::string_view msg) { log(LogLevel::DEBUG, m, msg); }
void Logger::fatal(std::string_view m, std::string_view msg) { log(LogLevel::FATAL, m, msg); }

// ── Private helpers ──────────────────────────────────────────────────────────

std::string Logger::timestamp_now() {
    using namespace std::chrono;
    auto now    = system_clock::now();
    auto ms     = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    auto timer  = system_clock::to_time_t(now);
    std::tm tm_buf{};
#if defined(_WIN32)
    gmtime_s(&tm_buf, &timer);
#else
    gmtime_r(&timer, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%FT%T")
        << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

std::string Logger::thread_id_str() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

} // namespace nombacrypt
