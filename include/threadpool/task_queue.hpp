#pragma once

#include "base_task.hpp"

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>


class TaskQueue {
    private:
        std::queue<std::unique_ptr<BaseTask>> tasks_;
        mutable std::mutex mutex_;
        std::condition_variable cv;

    public:
        TaskQueue() = default;

        TaskQueue(const TaskQueue&) = delete;
        TaskQueue& operator=(const TaskQueue&) = delete;

        void push(std::unique_ptr<BaseTask> task);

        std::unique_ptr<BaseTask> pop();

        size_t size() const;
        bool empty() const;
};
