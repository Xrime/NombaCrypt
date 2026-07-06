#pragma once


#include "buffer/transaction_slot.hpp"
#include "buffer/buffer_telemetry.hpp"
#include "core/constants.hpp"
#include <array>
#include <atomic>
#include <cstdint>

namespace nombacrypt {

class RingBuffer {
public:
    explicit RingBuffer(size_t capacity = BUFFER_CAPACITY);

    int64_t enqueue(const char* json_payload, uint32_t len, uint8_t priority = 0);
    TransactionSlot* dequeue_for_processing();

    /// Returns next slot in writen state for crypto verification (me, Security)
    TransactionSlot* dequeue_for_crypto_verification();

    void mark_dispatched(int64_t slot_index);

    BufferTelemetry get_telemetry() const;

private:
    alignas(64) std::array<TransactionSlot, BUFFER_CAPACITY> slots_;

    alignas(64) std::atomic<uint64_t> head_{0};
    alignas(64) std::atomic<uint64_t> tail_{0};
    std::atomic<uint64_t> total_enqueued_{0};
    std::atomic<uint64_t> total_dispatched_{0};

    size_t capacity_;
};

}