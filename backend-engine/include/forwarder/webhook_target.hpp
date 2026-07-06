#pragma once

#include <string>

namespace nombacrypt {


struct WebhookTarget {
    std::string target_id;


    std::string url;

    std::string secret_key;

    bool enabled = true;

    std::string description;

    WebhookTarget() = default;

    WebhookTarget(std::string id, std::string endpoint_url,
                  std::string key, bool is_enabled = true,
                  std::string desc = "")
        : target_id(std::move(id))
        , url(std::move(endpoint_url))
        , secret_key(std::move(key))
        , enabled(is_enabled)
        , description(std::move(desc))
    {}
};

}
