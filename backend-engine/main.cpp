#include "core/config.hpp"
#include "core/logger.hpp"
#include "buffer/ring_buffer.hpp"
#include "server/http_server.hpp"
#include <thread>
#include <memory>

using namespace nombacrypt;

#include "crypto/hmac_verifier.hpp"
#include "crypto/anti_replay_ledger.hpp"
#include "crypto/sequence_hasher.hpp"

// A temporary background thread to clear the buffer so it doesn't get stuck at 8,192
void mock_dispatcher(std::shared_ptr<RingBuffer> buffer) {
    while (true) {
        TransactionSlot* slot = buffer->dequeue_for_processing();
        if (slot) {
            // TEAMMATE: Verify the payload signature using the HMAC Verifier
            std::string payload_str(slot->payload, slot->payload_len);
            std::string signature_str(slot->signature);
            
            // Note: In production, load this from Config
            std::string secret_key = "nomba_secret_key"; 
            
            if (!HmacVerifier::verify_signature(payload_str, signature_str, secret_key)) {
                LOG_INFO("Dropped transaction " << slot->slot_id << " due to INVALID SIGNATURE");
            } else {
                // TEAMMATE: Check against replay attacks
                uint64_t hash = SequenceHasher::hash_transaction(payload_str);
                if (!AntiReplayLedger::get_instance().insert_and_check(hash, slot->ingested_at_us)) {
                    LOG_INFO("Dropped transaction " << slot->slot_id << " due to REPLAY ATTACK (Duplicate Hash)");
                } else {
                    // Transaction is authentic and unique!
                    // Proceed to send to Nomba Sandbox...
                }
            }

            buffer->mark_dispatched(slot->slot_id);
        } else {
            std::this_thread::yield();
        }
    }
}

int main() {
    Config::get_instance().load_from_env();
    LOG_INFO(" Starting NombaCrypt Shell Engine");

    // 1. Create the massive 34MB Ring Buffer on the Heap
    auto buffer = std::make_shared<RingBuffer>(Config::get_instance().buffer_capacity);

    // 2. Start the background consumer so the buffer drains automatically
    std::thread dispatcher(mock_dispatcher, buffer);
    dispatcher.detach();

    // 3. Start the Network Server on Port 8080!
    HttpServer server(8080, buffer);

    // This will block the thread forever, keeping the server alive and listening!
    server.start();

    return 0;
}