#include "crypto/event_ledger.hpp"
#include <iostream>
#include <chrono>

namespace nombacrypt {

EventLedger& EventLedger::get_instance() {
    static EventLedger instance;
    return instance;
}

EventLedger::EventLedger() : log_filepath_("security_audit.log") {
    log_file_.open(log_filepath_, std::ios::out | std::ios::app);
}

EventLedger::~EventLedger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void EventLedger::configure(const std::string& filepath) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_.close();
    }
    log_filepath_ = filepath;
    log_file_.open(log_filepath_, std::ios::out | std::ios::app);
}

void EventLedger::register_telemetry_callback(TelemetryCallback callback) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    telemetry_callbacks_.push_back(std::move(callback));
}

void EventLedger::log_event(const std::string& tx_id,
                            const std::string& event_type,
                            const std::string& status,
                            const std::string& details) noexcept {
    Timestamp now = now_microseconds();

    std::lock_guard<std::mutex> lock(mutex_);
    if (!log_file_.is_open()) {
        std::cerr << "[" << now << "] | " << event_type << " | " << tx_id << " | " << status << " | " << details << std::endl;
    } else {
        log_file_ << now << " | "
                  << event_type << " | "
                  << tx_id << " | "
                  << status << " | "
                  << details << "\n";
        log_file_.flush();
    }

    std::string json_event = "{"
        "\"type\": \"SECURITY_ALERT\", "
        "\"event\": \"" + event_type + "\", "
        "\"tx_id\": \"" + tx_id + "\", "
        "\"status\": \"" + status + "\", "
        "\"details\": \"" + details + "\", "
        "\"timestamp\": " + std::to_string(now) +
        "}";

    recent_events_.push_front(json_event);
    if (recent_events_.size() > 50) {
        recent_events_.pop_back();
    }

    // Broadcast to telemetry subscribers (WebSocket IPC Bridge)
    if (!telemetry_callbacks_.empty()) {
        for (const auto& callback : telemetry_callbacks_) {
            callback(json_event);
        }
    }
}

std::string EventLedger::get_recent_events_json() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string result = "[";
    for (size_t i = 0; i < recent_events_.size(); ++i) {
        result += recent_events_[i];
        if (i < recent_events_.size() - 1) {
            result += ",";
        }
    }
    result += "]";
    return result;
}

} // namespace nombacrypt
