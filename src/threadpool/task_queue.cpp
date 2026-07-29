#include "threadpool/task_queue.hpp"

#include <mutex>
#include <thread>


void TaskQueue::push(std::unique_ptr<BaseTask> task) {
    if (!task) {
        return;
    }

    {
        std::unique_lock lock(mutex_);
        tasks_.push(std::move(task));
    }
}

std::unique_ptr<BaseTask> TaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    while (tasks_.empty()) {
        lock.unlock();
        std::this_thread::yield();
        lock.lock();
    }

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
