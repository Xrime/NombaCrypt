#include "server/http_server.hpp"
#include "core/logger.hpp"
#include "core/config.hpp"

// Tell the MinGW compiler we are running on modern Windows 10+
#ifdef _WIN32
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0A00
#endif

#include "httplib.h"

namespace nombacrypt {

    HttpServer::HttpServer(uint16_t port, std::shared_ptr<RingBuffer> buffer)
        : port_(port), buffer_(buffer), svr_(std::make_unique<httplib::Server>()) {
        setup_routes();
    }

    HttpServer::~HttpServer() {
        stop();
    }

    void HttpServer::setup_routes() {
        svr_->Post("/api/accounts", [this](const httplib::Request& req, httplib::Response& res) {

            // TEAMMATE: Extract the X-Signature header from the request. Default to empty if missing.
            std::string signature = req.has_header("X-Signature") ? req.get_header_value("X-Signature") : "";

            // Attempt to shove the request payload instantly into the Lock-Free Buffer
            int64_t slot = buffer_->enqueue(req.body.c_str(), req.body.length(), signature.c_str());

            if (slot != -1) {
                // Buffer accepted it! Return 202 Accepted.
                res.status = 202;
                res.set_content("{\"status\":\"queued\"}", "application/json");
            } else {
                // Buffer is completely full (8,192 capacity maxed out)!
                res.status = 429;
                res.set_content("{\"error\":\"Too Many Requests\"}", "application/json");
            }
        });

        // Telemetry endpoint for the React Dashboard
        svr_->Get("/api/telemetry", [this](const httplib::Request& req, httplib::Response& res) {
            BufferTelemetry stats = buffer_->get_telemetry();
            std::string json = "{\"total_enqueued\":" + std::to_string(stats.total_enqueued) + "}";
            res.set_content(json, "application/json");
        });

        // -----------------------------------------------------------------------
        // GET /api/config - Retrieve dynamic configuration
        // -----------------------------------------------------------------------
        svr_->Get("/api/config", [](const httplib::Request&, httplib::Response& res) {
            const Config& config = Config::get_instance();
            std::string json_res = "{"
                "\"nomba_api_base_url\":\"" + config.get_nomba_api_base_url() + "\","
                "\"nomba_client_id\":\"" + config.get_nomba_client_id() + "\","
                "\"nomba_private_key\":\"" + config.get_nomba_private_key() + "\","
                "\"nomba_account_id\":\"" + config.get_nomba_account_id() + "\""
                "}";
            res.set_content(json_res, "application/json");
        });

        // -----------------------------------------------------------------------
        // POST /api/config - Update dynamic configuration
        // -----------------------------------------------------------------------
        svr_->Post("/api/config", [](const httplib::Request& req, httplib::Response& res) {
            Config& config = Config::get_instance();
            
            // Very basic JSON parsing without pulling in heavy dependencies.
            // In a production app, we would use RapidJSON or nlohmann/json.
            // We will do a simple string search for this hackathon feature.
            
            auto extract_json_val = [](const std::string& body, const std::string& key) -> std::string {
                std::string search = "\"" + key + "\":\"";
                size_t pos = body.find(search);
                if (pos == std::string::npos) return "";
                pos += search.length();
                size_t end_pos = body.find("\"", pos);
                if (end_pos == std::string::npos) return "";
                return body.substr(pos, end_pos - pos);
            };

            std::string url = extract_json_val(req.body, "nomba_api_base_url");
            std::string client = extract_json_val(req.body, "nomba_client_id");
            std::string key = extract_json_val(req.body, "nomba_private_key");
            std::string account = extract_json_val(req.body, "nomba_account_id");

            if (!url.empty()) config.set_nomba_api_base_url(url);
            if (!client.empty()) config.set_nomba_client_id(client);
            if (!key.empty()) config.set_nomba_private_key(key);
            if (!account.empty()) config.set_nomba_account_id(account);

            LOG_INFO("[HttpServer] Dynamic configuration updated via /api/config");
            res.set_content("{\"status\":\"success\"}", "application/json");
        });
    }

    void HttpServer::start() {
        LOG_INFO("HTTP Server starting on port " << port_ << "...");
        // Bind to 0.0.0.0 so Docker can expose the port to the public internet
        svr_->listen("0.0.0.0", port_);
    }

    void HttpServer::stop() {
        if (svr_->is_running()) {
            svr_->stop();
            LOG_INFO("HTTP Server stopped.");
        }
    }

} // namespace nombacrypt