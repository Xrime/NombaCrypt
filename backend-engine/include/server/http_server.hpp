#pragma once

#include "buffer/ring_buffer.hpp"
#include <memory>
#include <string>

// Forward declaration so we don't have to include the massive 8000-line header here
namespace httplib {
    class Server;
}

namespace nombacrypt {

class HttpServer {
public:
    HttpServer(uint16_t port, std::shared_ptr<RingBuffer> buffer);

    // Destructor to clean up the server when we exit
    ~HttpServer();

    void start();
    void stop();

private:
    void setup_routes();

    uint16_t port_;
    std::shared_ptr<RingBuffer> buffer_;
    std::unique_ptr<httplib::Server> svr_;
};

}