class BaseTask {
public:
    virtual ~BaseTask() = default;

    virtual void execute() = 0; 

    virtual const char* name() const = 0; 
};