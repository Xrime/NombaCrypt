#include "core/config.hpp"
#include "core/logger.hpp"
#include "buffer/ring_buffer.hpp"
#include "server/http_server.hpp"
#include "pipeline/crypto_worker.hpp"
#include <thread>
#include <memory>

using namespace nombacrypt;

// Background dispatcher: drains PROCESSING slots after CryptoWorker approves them
void dispatcher_thread(std::shared_ptr<RingBuffer> buffer) {
    while (true) {
        TransactionSlot* slot = buffer->dequeue_for_processing();
        if (slot) {
            // Transaction passed all security checks — mark it as dispatched
            buffer->mark_dispatched(slot->slot_id);
        } else {
            std::this_thread::yield();
        }
    }
}

int main() {
    Config::get_instance().load_from_env();
    LOG_INFO(" Starting NombaCrypt Shell Engine");

    // 1. Create the Ring Buffer
    auto buffer = std::make_shared<RingBuffer>(Config::get_instance().buffer_capacity);

    // 2. Start the CryptoWorker (security verification pipeline)
    //    This thread scans for WRITTEN slots, verifies HMAC signatures,
    //    checks for replay attacks, and logs everything to the EventLedger.
    CryptoWorker crypto(*buffer);
    std::stop_source stop_src;
    std::thread crypto_thread([&crypto, &stop_src]() {
        crypto(stop_src.get_token());
    });
    crypto_thread.detach();
    LOG_INFO("[Main] CryptoWorker security pipeline started");

    // 3. Start the dispatcher (drains approved transactions)
    std::thread disp(dispatcher_thread, buffer);
    disp.detach();
    LOG_INFO("[Main] Dispatcher thread started");

    // 4. Start the HTTP Server on Port 8080
    HttpServer server(8080, buffer);
    LOG_INFO("[Main] HTTP Server starting on port 8080...");
    server.start();

    return 0;
}