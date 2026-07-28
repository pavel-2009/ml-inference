#include "threadpool.hpp"
#include "base_task.hpp"

#include <iostream>


void ThreadPool::worker_loop() {
    while (!stop_flag_) {
        try
        {
            std::unique_lock lock(mutex_);

            cv_finished_.wait(lock, [&]{
                return active_tasks_ == 0;
            });

            auto task = tasks_.pop();

            if (!task) continue;

            task->execute();

            --active_tasks_;

            if (active_tasks_ == 0) {
                cv_finished_.notify_all();
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
    }
}

ThreadPool::ThreadPool(size_t num_threads, TaskQueue& task_queue)
    : tasks_(task_queue)
    , stop_flag_(false) 
{
    if (num_threads == 0) {
        num_threads = 1; 
    }

    workers_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this]() {
            this->worker_loop();
        });
    }
}

ThreadPool::~ThreadPool() {
    stop_flag_ = true;

    for (std::jthread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::unique_ptr<BaseTask> task) {
    tasks_.push(std::move(task));
    ++active_tasks_;
}

size_t ThreadPool::size() const {
    return workers_.size();
}