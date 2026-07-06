#pragma once

#include "core/types.hpp"
#include <string>
#include <mutex>
#include <fstream>
#include <vector>
#include <functional>

namespace nombacrypt {

class EventLedger {
public:
    using TelemetryCallback = std::function<void(const std::string&)>;

    static EventLedger& get_instance();

    EventLedger(const EventLedger&) = delete;
    EventLedger& operator=(const EventLedger&) = delete;
    EventLedger(EventLedger&&) = delete;
    EventLedger& operator=(EventLedger&&) = delete;

    void configure(const std::string& filepath) noexcept;

    void register_telemetry_callback(TelemetryCallback callback) noexcept;

    void log_event(const std::string& tx_id,
                   const std::string& event_type,
                   const std::string& status,
                   const std::string& details) noexcept;

private:
    EventLedger();
    ~EventLedger();

    std::mutex mutex_;
    std::string log_filepath_;
    std::ofstream log_file_;
    std::vector<TelemetryCallback> telemetry_callbacks_;
};

} // namespace nombacrypt
