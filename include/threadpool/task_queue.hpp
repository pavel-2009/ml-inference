#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>


class BaseTask {
public:
    virtual ~BaseTask() = default;

    virtual void execute() = 0; 

    virtual const char* name() const = 0; 
};


class TaskQueue {
    private:
        std::queue<std::unique_ptr<BaseTask>> tasks_;
        std::mutex mutex_;
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
