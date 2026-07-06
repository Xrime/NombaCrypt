#pragma once

#include "core/types.hpp"
#include <atomic>
#include <cstring>

namespace nombacrypt {

struct alignas(64) TransactionSlot {
    SlotIndex    slot_id;                       ///< Sequential ID of the transaction
    char         payload[MAX_PAYLOAD_SIZE];     ///< Raw JSON bytes
    uint32_t     payload_len;                   ///< Actual number of bytes used
    char         signature[65];                 // TEAMMATE: Contains the X-Signature header from the API for verification
    Timestamp    ingested_at_us;                ///< When this hit our server
    uint8_t      priority;                      ///< 0=Normal, 1=High (for retries)
    std::atomic<SlotState> state;               ///< Thread-safe state tracker

    TransactionSlot()
        : slot_id(0),
          payload_len(0),
          ingested_at_us(0),
          priority(0),
          state(SlotState::EMPTY)
    {
        std::memset(payload, 0, MAX_PAYLOAD_SIZE);
        std::memset(signature, 0, sizeof(signature));
    }
};

}