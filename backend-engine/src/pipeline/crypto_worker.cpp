/**
 * @file crypto_worker.cpp
 * @brief Implementation of the CryptoWorker — security verification pipeline stage.
 */

#include "pipeline/crypto_worker.hpp"
#include <thread>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <emmintrin.h>
#endif

namespace nombacrypt {

CryptoWorker::CryptoWorker(RingBuffer& buffer)
    : buffer_(buffer) {}

void CryptoWorker::operator()(std::stop_token st) {
    LOG_INFO("[CryptoWorker] Security verification thread started");

    while (!st.stop_requested()) {
        TransactionSlot* slot = find_written_slot();

        if (slot) {
            if (enabled_.load(std::memory_order_relaxed)) {
                // Full security verification
                bool passed = verify_slot(slot);

                if (passed) {
                    // Approved: transition to PROCESSING for dispatch workers
                    slot->state.store(SlotState::PROCESSING, std::memory_order_release);
                    stat_verified_.fetch_add(1, std::memory_order_relaxed);
                } else {
                    // Rejected: drop the slot back to EMPTY
                    slot->state.store(SlotState::EMPTY, std::memory_order_release);
                }
            } else {
                // Crypto Shield disabled (Toggle OFF): auto-approve everything
                slot->state.store(SlotState::PROCESSING, std::memory_order_release);
                stat_verified_.fetch_add(1, std::memory_order_relaxed);
            }
        } else {
            // No WRITTEN slots found — yield to avoid busy-spinning
            #ifdef _WIN32
                YieldProcessor();
            #else
                _mm_pause();
            #endif
        }
    }

    LOG_INFO("[CryptoWorker] Security verification thread stopping");
}

bool CryptoWorker::verify_slot(TransactionSlot* slot) {
    std::string payload(slot->payload, slot->payload_len);
    std::string slot_id_str = std::to_string(slot->slot_id);

    // Step 1: Compute the payload hash for anti-replay check
    uint64_t payload_hash = SequenceHasher::fnv1a_hash(slot->payload, slot->payload_len);

    // Step 2: Anti-replay check — reject duplicate payloads within TTL window
    bool is_unique = AntiReplayLedger::get_instance().insert_and_check(payload_hash);
    if (!is_unique) {
        stat_rejected_replay_.fetch_add(1, std::memory_order_relaxed);
        EventLedger::get_instance().log_event(
            slot_id_str,
            "REPLAY_ATTACK_BLOCKED",
            "FAILED",
            "Duplicate payload hash detected"
        );
        return false;
    }

    // Step 3: HMAC signature verification
    // In production, the signature would come from an HTTP header (X-Nomba-Signature).
    // For the hackathon pipeline, we compute the expected signature using the configured
    // private key, but in production, we would verify a client-provided signature.

    // 1. Signature Verification (Simulated with our own key for the demo)
    const Config& config = Config::get_instance();
    std::string private_key = config.get_nomba_private_key();

    // Multi-API Load Balancing logic (Round-Robin)
    if (config.get_multi_api_mode()) {
        static std::atomic<uint64_t> rr_counter{0};
        uint64_t current = rr_counter.fetch_add(1, std::memory_order_relaxed);
        
        int channel = current % 3;
        std::string channel_name = "Primary";
        if (channel == 1) {
            std::string key_b = config.get_channel_b_private_key();
            if (!key_b.empty()) {
                private_key = key_b;
                channel_name = "Channel B";
            }
        } else if (channel == 2) {
            std::string key_c = config.get_channel_c_private_key();
            if (!key_c.empty()) {
                private_key = key_c;
                channel_name = "Channel C";
            }
        }
        // Very verbose for debugging, maybe we shouldn't log on every request. 
        // But for hackathon demonstration purposes, let's keep it.
        LOG_INFO("[CryptoWorker] Load Balancer selected " << channel_name);
    }

    if (!private_key.empty()) {
        // Compute HMAC signature of the payload for integrity verification
        std::string computed_sig = HmacVerifier::compute_hmac_hex(payload, private_key);
        std::string provided_sig = slot->signature;

        if (provided_sig == computed_sig) {
            // Log the successful verification
            EventLedger::get_instance().log_event(
                slot_id_str,
                "SIGNATURE_VERIFIED",
                "SUCCESS",
                "HMAC: " + computed_sig.substr(0, 16) + "..."
            );
        } else {
            // HMAC mismatch! Log it and mark slot as empty to drop it.
            EventLedger::get_instance().log_event(
                slot_id_str,
                "HMAC_FAIL",
                "BLOCKED",
                "Provided: " + (provided_sig.empty() ? "NONE" : provided_sig.substr(0, 8) + "...") + " | Expected: " + computed_sig.substr(0, 8) + "..."
            );
            stat_rejected_sig_.fetch_add(1, std::memory_order_relaxed);
            
            // DROP the transaction by resetting slot to EMPTY
            // The caller will also set it to EMPTY, but returning false is the key.
            return false;
        }
    }

    // Step 4: Log successful verification
    EventLedger::get_instance().log_event(
        slot_id_str,
        "TRANSACTION_VERIFIED",
        "SUCCESS",
        "Hash: " + std::to_string(payload_hash)
    );

    return true;
}

TransactionSlot* CryptoWorker::find_written_slot() {
    return buffer_.dequeue_for_crypto_verification();
}

void CryptoWorker::set_enabled(bool enabled) noexcept {
    enabled_.store(enabled, std::memory_order_relaxed);
    LOG_INFO("[CryptoWorker] Crypto Shield " << (enabled ? "ENABLED" : "DISABLED"));
}

bool CryptoWorker::is_enabled() const noexcept {
    return enabled_.load(std::memory_order_relaxed);
}

CryptoWorker::Stats CryptoWorker::get_stats() const noexcept {
    return {
        stat_verified_.load(std::memory_order_relaxed),
        stat_rejected_sig_.load(std::memory_order_relaxed),
        stat_rejected_replay_.load(std::memory_order_relaxed)
    };
}

} // namespace nombacrypt
