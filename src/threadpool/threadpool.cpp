#include "threadpool/threadpool.hpp"
#include "base_task.hpp"

#include <iostream>


void ThreadPool::stop() noexcept {
    stop_flag_ = true;
    
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
    }
    cv_.notify_all();
}

void ThreadPool::worker_loop() {
    while (true) {
        std::unique_ptr<BaseTask> task;
        
        {
            std::unique_lock<std::mutex> lock(cv_mutex_);
            
            cv_.wait(lock, [this] {
                return stop_flag_ || !tasks_.empty();
            });
            
            if (stop_flag_ && tasks_.empty()) {
                return;
            }
            
            task = tasks_.pop_no_wait();
        }
        
        if (!task) {
            continue;
        }
        
        try {
            task->execute();
        } catch (const std::exception& e) {
            std::cerr << "Task exception: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Task unknown exception\n";
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
    stop();
}

void ThreadPool::enqueue(std::unique_ptr<BaseTask> task) {
    if (!task) {
        return;
    }
    
    tasks_.push(std::move(task));
    
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
    }
    cv_.notify_one();
}

size_t ThreadPool::size() const {
    return workers_.size();
}