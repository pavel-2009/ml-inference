class BaseTask {
public:
    virtual ~BaseTask() = default;

    virtual void execute() = 0; 
};