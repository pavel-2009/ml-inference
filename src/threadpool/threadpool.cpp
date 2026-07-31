#include "threadpool/threadpool.hpp"
#include "base_task.hpp"

#include <iostream>


void ThreadPool::stop() noexcept {
    stop_flag_ = true;
    
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
    }
    cv_.notify_all();
    tasks_.notify();
}

void ThreadPool::worker_loop() {
    while (!stop_flag_) {
        std::unique_ptr<BaseTask> task;
        
        {
            std::unique_lock lock(cv_mutex_);

            cv_.wait(lock, [this]{
                return stop_flag_ || !tasks_.empty();
            });

            if (stop_flag_ && tasks_.empty()) {
                break;
            }

            task = tasks_.pop_no_wait();
        }

        if (!task) {
            continue;
        }

        try {
            task->execute();
        } catch (const std::exception& e) {
            std::cerr << "Task execution error: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown task execution error" << std::endl;
        }

        --active_tasks_;

        if (active_tasks_ == 0) {
            cv_.notify_all();
        }
    }
}

ThreadPool::ThreadPool(size_t num_threads, TaskQueue& task_queue)
    : tasks_(task_queue), 
      stop_flag_(false) 
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
    
    ++active_tasks_;
    tasks_.push(std::move(task));
}

size_t ThreadPool::size() const {
    return workers_.size();
}

void ThreadPool::wait() {
    std::unique_lock lock(cv_mutex_);
    cv_.wait(lock, [this] {
        return active_tasks_ == 0;
    });
}