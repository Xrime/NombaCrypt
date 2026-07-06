#pragma once

#include "core/types.hpp"
#include "forwarder/webhook_target.hpp"

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

namespace nombacrypt {

class DownstreamForwarder {
public:
    static DownstreamForwarder& get_instance();

    // Non-copyable, non-movable
    DownstreamForwarder(const DownstreamForwarder&) = delete;
    DownstreamForwarder& operator=(const DownstreamForwarder&) = delete;
    DownstreamForwarder(DownstreamForwarder&&) = delete;
    DownstreamForwarder& operator=(DownstreamForwarder&&) = delete;

    void add_target(WebhookTarget target);

    bool remove_target(const std::string& target_id);

    bool forward(const std::string& tx_id, const std::string& payload);

    void set_enabled(bool enabled);

    bool is_enabled() const;

    std::vector<WebhookTarget> list_targets() const;

    size_t target_count() const;

private:
    DownstreamForwarder();
    ~DownstreamForwarder() = default;

    mutable std::mutex mutex_;
    bool enabled_ = true;

    std::unordered_map<std::string, WebhookTarget> targets_;

    bool dispatch_to_target(const WebhookTarget& target,
                            const std::string& tx_id,
                            const std::string& sanitized_payload,
                            const std::string& signature);
};

}
