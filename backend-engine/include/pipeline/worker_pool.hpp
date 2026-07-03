#pragma once

/**
 * @file worker_pool.hpp
 * @brief Generic thread-pool that drives each pipeline stage.
 *
 * Each WorkerPool owns N std::jthreads and invokes a user-supplied callable
 * in a loop until stop is requested.
 */

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace nombacrypt {

/**
 * @class WorkerPool
 * @brief Configurable pool of worker threads for a single pipeline stage.
 */
class WorkerPool {
public:
    /// The callable signature each worker will invoke in a spin/wait loop.
    using WorkerFn = std::function<void(std::stop_token)>;

    /**
     * @brief Construct a worker pool.
     * @param name         Human-readable name (e.g. "ingest", "crypto", "dispatch").
     * @param thread_count Number of worker threads.
     * @param fn           Function each thread will run.
     */
    WorkerPool(std::string name, uint32_t thread_count, WorkerFn fn);
    ~WorkerPool();

    // Non-copyable / non-movable.
    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;

    /// Start all worker threads.
    void start();

    /// Request graceful stop and join all threads.
    void stop();

    /// @return true if threads are currently running.
    [[nodiscard]] bool is_running() const noexcept;

    /// @return the configured thread count.
    [[nodiscard]] uint32_t thread_count() const noexcept;

    /// @return human-readable pool name.
    [[nodiscard]] const std::string& name() const noexcept;

private:
    std::string              name_;
    uint32_t                 thread_count_;
    WorkerFn                 fn_;
    std::vector<std::jthread> threads_;
    std::atomic<bool>        running_{false};
};

} // namespace nombacrypt
