#pragma once

//gafar this is for you


#include <cstddef>
#include <cstdint>

namespace nombacrypt {

/**
   Snapshot of the buffer's current state.
 */
struct BufferTelemetry {
    size_t   active_count;       ///< How many requests are currently sitting in the queue
    float    capacity_percent;   ///< How full the buffer is (0.0 to 100.0)
    uint64_t total_enqueued;     ///< Total requests ingested since the server started
    uint64_t total_dispatched;   ///< Total requests sent out to Nomba successfully
};

}