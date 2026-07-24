#include "threadpool/task_queue.hpp"

#include <mutex>


void TaskQueue::push(std::unique_ptr<BaseTask> task) {
    if (!task) {
        return;
    }

    {
        std::unique_lock lock(mutex_);

        tasks_.push(std::move(task));
    };

    cv.notify_one();
}

std::unique_ptr<BaseTask> TaskQueue::pop() {

    std::unique_lock<std::mutex> lock(mutex_);

    cv.wait(lock, [this]{
        return !tasks_.empty();
    });

    auto task = std::move(tasks_.front());
    tasks_.pop();

    return task;
}

size_t TaskQueue::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

bool TaskQueue::empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.empty();
}
