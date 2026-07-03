#pragma once

#include <string>
#include <sstream>
#include <mutex>

namespace nombacrypt{

enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& get_instance();

    void log(LogLevel level, const std::string& message);

private:
    Logger() = default;

    std::mutex mutex_;
};

#define LOG_INFO(msg) do { \
    std::ostringstream _nombacrypt_oss; \
    _nombacrypt_oss << msg; \
    nombacrypt::Logger::get_instance().log(nombacrypt::LogLevel::INFO, _nombacrypt_oss.str()); \
} while(0)

#define LOG_WARN(msg) do { \
    std::ostringstream _nombacrypt_oss; \
    _nombacrypt_oss << msg; \
    nombacrypt::Logger::get_instance().log(nombacrypt::LogLevel::WARN, _nombacrypt_oss.str()); \
} while(0)

#define LOG_ERROR(msg) do { \
    std::ostringstream _nombacrypt_oss; \
    _nombacrypt_oss << msg; \
    nombacrypt::Logger::get_instance().log(nombacrypt::LogLevel::ERROR, _nombacrypt_oss.str()); \
} while(0)

}