#include "core/config.hpp"
#include "core/logger.hpp"
#include "buffer/ring_buffer.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

using namespace nombacrypt;

// The PRODUCER: Blasts requests into the buffer
void simulate_traffic_spike(std::shared_ptr<RingBuffer> buffer, int thread_id, int requests_to_send) {
    std::string dummy_json = R"({"student_id": "UI/2026/123", "amount": 50000, "currency": "NGN"})";

    for (int i = 0; i < requests_to_send; ++i) {
        // Spin-wait if the buffer is temporarily full
        while (buffer->enqueue(dummy_json.c_str(), dummy_json.length()) == -1) {
            std::this_thread::yield();
        }
    }
    LOG_INFO("Traffic Thread " << thread_id << " finished sending " << requests_to_send << " requests.");
}

// The CONSUMER: Reads requests and empties the buffer
void simulate_consumer(std::shared_ptr<RingBuffer> buffer, int total_expected) {
    int processed = 0;
    while (processed < total_expected) {
        // Now it will successfully pull the requests out!
        TransactionSlot* slot = buffer->dequeue_for_processing();
        if (slot) {
            // Mark it dispatched, which frees up the space for new traffic
            buffer->mark_dispatched(slot->slot_id);
            processed++;
        } else {
            std::this_thread::yield(); // Buffer empty, wait a microsecond
        }
    }
    LOG_INFO("Consumer Thread finished. Dequeued " << processed << " requests.");
}

int main() {
    Config::get_instance().load_from_env();
    LOG_INFO(" Starting NombaCrypt Shell Engine ");

    auto buffer = std::make_shared<RingBuffer>(Config::get_instance().buffer_capacity);

    const int num_threads = 4;

    // Let's go for the full 1,000,000 requests!
    const int requests_per_thread = 250000;
    const int total_requests = num_threads * requests_per_thread;

    LOG_WARN("Starting Stress Test: Ingesting " << total_requests << " concurrent requests...");

    auto start_time = std::chrono::high_resolution_clock::now();

    // Launch the Consumer first so it's ready to watch the buffer
    std::thread consumer(simulate_consumer, buffer, total_requests);

    // Launch the Producers to blast the traffic
    std::vector<std::thread> producers;
    for (int i = 0; i < num_threads; ++i) {
        producers.emplace_back(simulate_traffic_spike, buffer, i, requests_per_thread);
    }

    // Wait for the traffic blast to finish
    for (auto& t : producers) {
        t.join();
    }
    consumer.join();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    BufferTelemetry stats = buffer->get_telemetry();

    LOG_INFO(" STRESS TEST COMPLETE ");
    LOG_INFO(" Total Ingested: " << stats.total_enqueued);
    LOG_INFO(" Time Taken:     " << duration_ms << " ms");

    // Calculate throughput
    double seconds = duration_ms / 1000.0;
    if (seconds > 0) {
        LOG_INFO(" Throughput:     " << (int)(stats.total_enqueued / seconds) << " requests / second");
    }


    return 0;
}