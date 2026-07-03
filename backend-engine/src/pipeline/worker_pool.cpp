/**
 * @file worker_pool.cpp
 * @brief Implementation of nombacrypt::WorkerPool.
 */

#include "pipeline/worker_pool.hpp"
#include "core/logger.hpp"

namespace nombacrypt {

WorkerPool::WorkerPool(std::string name, uint32_t thread_count, WorkerFn fn)
    : name_(std::move(name))
    , thread_count_(thread_count)
    , fn_(std::move(fn)) {}

WorkerPool::~WorkerPool() {
    stop();
}

void WorkerPool::start() {
    if (running_.exchange(true)) return; // already running

    threads_.reserve(thread_count_);
    for (uint32_t i = 0; i < thread_count_; ++i) {
        threads_.emplace_back([this](std::stop_token st) {
            LOG_INFO("WorkerPool", "Thread started in pool '" + name_ + "'");
            fn_(st);
            LOG_INFO("WorkerPool", "Thread exiting pool '" + name_ + "'");
        });
    }

    LOG_INFO("WorkerPool", "Pool '" + name_ + "' started with " +
             std::to_string(thread_count_) + " threads");
}

void WorkerPool::stop() {
    if (!running_.exchange(false)) return; // already stopped

    for (auto& t : threads_) {
        t.request_stop();
    }
    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }
    threads_.clear();

    LOG_INFO("WorkerPool", "Pool '" + name_ + "' stopped");
}

bool WorkerPool::is_running() const noexcept {
    return running_.load(std::memory_order_relaxed);
}

uint32_t WorkerPool::thread_count() const noexcept {
    return thread_count_;
}

const std::string& WorkerPool::name() const noexcept {
    return name_;
}

} // namespace nombacrypt
