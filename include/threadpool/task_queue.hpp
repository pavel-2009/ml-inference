#pragma once

#include "base_task.hpp"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>


class TaskQueue {
    private:
        std::queue<std::unique_ptr<BaseTask>> tasks_;
        mutable std::mutex mutex_;
        std::condition_variable cv_;

    public:
        TaskQueue() = default;

        TaskQueue(const TaskQueue&) = delete;
        TaskQueue& operator=(const TaskQueue&) = delete;

        void push(std::unique_ptr<BaseTask> task);

        std::unique_ptr<BaseTask> pop();

        std::unique_ptr<BaseTask> pop_no_wait();

        size_t size() const;
        bool empty() const;

        void notify();
};
