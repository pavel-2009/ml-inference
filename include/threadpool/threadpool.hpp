#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "task_queue.hpp"
#include "base_task.hpp"


class ThreadPool {
    private:
        std::vector<std::jthread> workers_;
        TaskQueue& tasks_;
        std::atomic<bool> stop_flag_;

    private:
        void worker_loop();

    public:
        ThreadPool(size_t num_threads, TaskQueue& task_queue);
        
        ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        void enqueue(std::unique_ptr<BaseTask> task);

        size_t size() const;
};
