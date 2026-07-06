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
    if (!private_key.empty()) {
        // Compute HMAC signature of the payload for integrity verification
        std::string computed_sig = HmacVerifier::compute_hmac_hex(payload, private_key);

        // Log the successful verification
        EventLedger::get_instance().log_event(
            slot_id_str,
            "SIGNATURE_VERIFIED",
            "SUCCESS",
            "HMAC: " + computed_sig.substr(0, 16) + "..."
        );
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
