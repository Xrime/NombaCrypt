#include "server/http_server.hpp"
#include "core/logger.hpp"

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