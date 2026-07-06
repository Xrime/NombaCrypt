/**
 * @file downstream_forwarder.cpp
 * @brief Implementation of Toggle #7 — Downstream Forwarder.
 *
 * Manages registered webhook targets, sanitizes payloads, computes
 * HMAC-SHA256 signatures per target, and simulates HTTP dispatch.
 * All forwarding events are recorded in the EventLedger.
 */

#include "forwarder/downstream_forwarder.hpp"
#include "forwarder/payload_sanitizer.hpp"
#include "crypto/hmac_verifier.hpp"
#include "crypto/event_ledger.hpp"
#include "core/logger.hpp"

#include <sstream>

namespace nombacrypt {

// ---------------------------------------------------------------------------
// Singleton access
// ---------------------------------------------------------------------------
DownstreamForwarder& DownstreamForwarder::get_instance() {
    static DownstreamForwarder instance;
    return instance;
}

DownstreamForwarder::DownstreamForwarder()
    : enabled_(true)
{}

// ---------------------------------------------------------------------------
// add_target
// ---------------------------------------------------------------------------
void DownstreamForwarder::add_target(WebhookTarget target) {
    std::lock_guard<std::mutex> lock(mutex_);

    const std::string id = target.target_id;
    targets_[id] = std::move(target);

    LOG_INFO("[DownstreamForwarder] Target registered: " << id
             << " -> " << targets_[id].url);
}

// ---------------------------------------------------------------------------
// remove_target
// ---------------------------------------------------------------------------
bool DownstreamForwarder::remove_target(const std::string& target_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = targets_.find(target_id);
    if (it == targets_.end()) {
        LOG_WARN("[DownstreamForwarder] Cannot remove target '"
                 << target_id << "': not found");
        return false;
    }

    targets_.erase(it);
    LOG_INFO("[DownstreamForwarder] Target removed: " << target_id);
    return true;
}

// ---------------------------------------------------------------------------
// set_enabled / is_enabled
// ---------------------------------------------------------------------------
void DownstreamForwarder::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
    LOG_INFO("[DownstreamForwarder] Forwarder "
             << (enabled_ ? "ENABLED" : "DISABLED"));
}

bool DownstreamForwarder::is_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

// ---------------------------------------------------------------------------
// list_targets
// ---------------------------------------------------------------------------
std::vector<WebhookTarget> DownstreamForwarder::list_targets() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<WebhookTarget> result;
    result.reserve(targets_.size());
    for (const auto& [id, target] : targets_) {
        result.push_back(target);
    }
    return result;
}

// ---------------------------------------------------------------------------
// target_count
// ---------------------------------------------------------------------------
size_t DownstreamForwarder::target_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return targets_.size();
}

// ---------------------------------------------------------------------------
// dispatch_to_target — simulate HTTP POST to a single target
// ---------------------------------------------------------------------------
bool DownstreamForwarder::dispatch_to_target(
        const WebhookTarget& target,
        const std::string& tx_id,
        const std::string& sanitized_payload,
        const std::string& signature) {

    // In a production build this would perform an actual HTTP POST.
    // For the hackathon we simulate success and log the dispatch details.

    LOG_INFO("[DownstreamForwarder] DISPATCH tx=" << tx_id
             << " target=" << target.target_id
             << " url=" << target.url
             << " sig=" << signature.substr(0, 16) << "...");

    // Log to the cryptographic event ledger
    std::ostringstream details;
    details << "target=" << target.target_id
            << " url=" << target.url
            << " payload_bytes=" << sanitized_payload.size()
            << " hmac=" << signature.substr(0, 16) << "...";

    EventLedger::get_instance().log_event(
        tx_id,
        "DOWNSTREAM_FORWARD",
        "SUCCESS",
        details.str());

    return true;  // Simulated success
}

// ---------------------------------------------------------------------------
// forward — main entry point
// ---------------------------------------------------------------------------
bool DownstreamForwarder::forward(const std::string& tx_id,
                                   const std::string& payload) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Check: forwarder must be enabled
    if (!enabled_) {
        LOG_WARN("[DownstreamForwarder] Forwarder disabled, skipping tx="
                 << tx_id);
        return false;
    }

    // Check: must have at least one target
    if (targets_.empty()) {
        LOG_WARN("[DownstreamForwarder] No targets registered, skipping tx="
                 << tx_id);
        return false;
    }

    // Step 1: Sanitize the payload
    std::string sanitized = PayloadSanitizer::sanitize(payload);
    if (sanitized.empty()) {
        LOG_WARN("[DownstreamForwarder] Payload sanitization failed for tx="
                 << tx_id);

        EventLedger::get_instance().log_event(
            tx_id,
            "DOWNSTREAM_FORWARD",
            "REJECTED",
            "Payload failed sanitization");

        return false;
    }

    // Step 2: Forward to each enabled target
    size_t forwarded_count = 0;
    Timestamp forward_time = now_microseconds();

    for (const auto& [id, target] : targets_) {
        if (!target.enabled) {
            LOG_INFO("[DownstreamForwarder] Target '" << id
                     << "' is disabled, skipping");
            continue;
        }

        // Compute HMAC-SHA256 signature using the target's secret key
        std::string signature = HmacVerifier::compute_hmac_hex(
            sanitized, target.secret_key);

        // Dispatch (simulated HTTP POST)
        bool ok = dispatch_to_target(target, tx_id, sanitized, signature);
        if (ok) {
            ++forwarded_count;
        } else {
            LOG_WARN("[DownstreamForwarder] Dispatch failed for target '"
                     << id << "' tx=" << tx_id);

            EventLedger::get_instance().log_event(
                tx_id,
                "DOWNSTREAM_FORWARD",
                "FAILED",
                "Dispatch to target " + id + " failed");
        }
    }

    Timestamp elapsed = now_microseconds() - forward_time;
    LOG_INFO("[DownstreamForwarder] tx=" << tx_id
             << " forwarded to " << forwarded_count << "/"
             << targets_.size() << " targets in "
             << elapsed << " us");

    return forwarded_count > 0;
}

} // namespace nombacrypt
