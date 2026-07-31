#include "threadpool/task_queue.hpp"

#include <mutex>
#include <iostream>


void TaskQueue::push(std::unique_ptr<BaseTask> task) {
    if (!task) {
        std::cout << "⚠️ push: task is null" << std::endl;
        return;
    }

    std::cout << "📥 push: adding task to queue" << std::endl;
    {
        std::unique_lock lock(mutex_);
        tasks_.push(std::move(task));
        std::cout << "✅ push: task in queue, size=" << tasks_.size() << std::endl;
    }
    cv_.notify_one();
    std::cout << "🔔 push: notified" << std::endl;
}

std::unique_ptr<BaseTask> TaskQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    std::cout << "⏳ pop: waiting for task..." << std::endl;
    cv_.wait(lock, [this] { 
        bool has_task = !tasks_.empty();
        if (has_task) {
            std::cout << "✅ pop: task available, size=" << tasks_.size() << std::endl;
        }
        return has_task; 
    });

    auto task = std::move(tasks_.front());
    tasks_.pop();
    std::cout << "📤 pop: got task, queue size=" << tasks_.size() << std::endl;

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
