#include "crypto/hmac_verifier.hpp"
#include "crypto/anti_replay_ledger.hpp"
#include "crypto/sequence_hasher.hpp"
#include "core/types.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <cassert>
#include <string>

using namespace nombacrypt;

const std::string SECRET_KEY = "nombacrypt_production_secret_key_for_stress_testing";
const std::string PAYLOAD_TEMPLATE = "{\"transaction_id\": \"tx_%d\", \"amount\": 5000, \"currency\": \"NGN\"}";

std::atomic<uint64_t> total_verified{0};

void stress_worker(int num_iterations, int thread_id) {
    char buffer[256];
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate unique payload to prevent anti-replay blocks within the thread
        snprintf(buffer, sizeof(buffer), "{\"transaction_id\": \"tx_%d_%d\", \"amount\": 5000, \"currency\": \"NGN\"}", thread_id, i);
        std::string payload(buffer);
        
        // 1. HMAC generation and verification
        std::string signature = HmacVerifier::compute_hmac_hex(payload, SECRET_KEY);
        bool sig_valid = HmacVerifier::verify_signature(payload, signature, SECRET_KEY);
        
        // 2. Hash and Anti-Replay Ledger check
        uint64_t hash = SequenceHasher::fnv1a_hash(payload.c_str(), payload.length());
        bool is_unique = AntiReplayLedger::get_instance().insert_and_check(hash);
        
        if (sig_valid && is_unique) {
            total_verified.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

int main() {
    std::cout << "[Stress Test] Starting Security Pipeline Stress Test...\n";
    
    const int num_threads = 32;
    const int iterations_per_thread = 35000; // 35k * 32 threads = 1.12M requests
    const uint64_t total_expected = num_threads * iterations_per_thread;
    
    std::cout << "[Stress Test] Threads: " << num_threads << "\n";
    std::cout << "[Stress Test] Target Requests: " << total_expected << "\n";
    
    std::vector<std::thread> workers;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_threads; ++i) {
        workers.emplace_back(stress_worker, iterations_per_thread, i);
    }
    
    for (auto& w : workers) {
        w.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    uint64_t actual_verified = total_verified.load();
    double seconds = duration.count() / 1000.0;
    double tps = actual_verified / seconds;
    
    std::cout << "\n---------------------------------------\n";
    std::cout << "STRESS TEST RESULTS\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Time Elapsed  : " << duration.count() << " ms\n";
    std::cout << "Total Requests: " << actual_verified << "\n";
    std::cout << "Throughput    : " << (uint64_t)tps << " req/sec\n";
    std::cout << "Target Status : " << (tps >= 200000.0 ? "PASSED (>200k req/s) \xE2\x9C\x85" : "FAILED (<200k req/s) \xE2\x9D\x8C") << "\n";
    std::cout << "---------------------------------------\n\n";
    
    assert(actual_verified == total_expected);
    
    return (tps >= 200000.0) ? 0 : 1;
}
