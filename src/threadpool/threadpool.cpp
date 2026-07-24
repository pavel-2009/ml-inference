#include "threadpool.hpp"

#include <iostream>


void ThreadPool::worker_loop() {
    while (!stop_flag_) {
        try
        {
            auto task = tasks_.pop();

            if (!task) continue;

            task->execute();
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

size_t ThreadPool::size() const {
    return workers_.size();
}