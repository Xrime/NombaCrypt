#include "../../include/core/logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

namespace nombacrypt {

    Logger& Logger::get_instance() {
        static Logger instance;
        return instance;
    }

    void Logger::log(LogLevel level, const std::string& message) {
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        // Format the level string
        std::string level_str;
        switch(level) {
            case LogLevel::INFO:  level_str = "INFO "; break;
            case LogLevel::WARN:  level_str = "WARN "; break;
            case LogLevel::ERROR: level_str = "ERROR"; break;
        }


        // No other thread can pass this line until the current thread finishes printing.
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S")
                  << " | Thread " << std::this_thread::get_id() << " | "
                  << level_str << " " << message << "\n";
    }

}