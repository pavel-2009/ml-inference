#include "threadpool/task_queue.hpp"

#include <mutex>


void TaskQueue::push(std::unique_ptr<BaseTask> task) {
    if (!task) {
        return;
    }

    {
        std::lock_guard lock(mutex_);

        tasks_.push(std::move(task));
    };

    cv.notify_one();
}

std::unique_ptr<BaseTask> TaskQueue::pop() {

    std::lock_guard lock(mutex_);

    cv.wait(lock, [this]{
        return !tasks_.empty();
    });

    auto task = std::move(tasks_.front());
    tasks_.pop();

    return task;
}
