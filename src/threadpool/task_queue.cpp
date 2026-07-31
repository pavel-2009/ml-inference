#include "threadpool/task_queue.hpp"

#include <mutex>


void TaskQueue::push(std::unique_ptr<BaseTask> task) {
    if (!task) {
        return;
    }

    {
        std::unique_lock lock(mutex_);
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
}

std::unique_ptr<BaseTask> TaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    cv_.wait(lock, [this] { return !tasks_.empty(); });

    auto task = std::move(tasks_.front());
    tasks_.pop();

    return task;
}

std::unique_ptr<BaseTask> TaskQueue::pop_no_wait() {
    std::unique_lock lock(mutex_);
    
    if (tasks_.empty()) {
        return nullptr;
    }

    auto task = std::move(tasks_.front());
    tasks_.pop();

    return task;
}

size_t TaskQueue::size() const {
    std::unique_lock lock(mutex_);
    return tasks_.size();
}

bool TaskQueue::empty() const {
    std::unique_lock lock(mutex_);
    return tasks_.empty();
}

void TaskQueue::notify() {
    cv_.notify_all();
}
