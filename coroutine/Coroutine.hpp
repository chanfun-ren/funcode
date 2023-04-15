#include <ucontext.h>
#include <functional>
#include <vector>
#include <memory>
#include <cstring>
#include <cassert>

#define STACKSIZE (1024*8)

class Scheduler;
class Coroutine;

using Func = std::function<void(Scheduler*, void*)>;


enum CoState {
    DEAD,
    RUNNABLE,   
    RUNNING,
    SUSPEND
};

class Scheduler {
public:
    // 主上下文
    ucontext_t mainctx_;
    
    // 协程数组
    std::vector<std::shared_ptr<Coroutine>> cotines_;

    // 正在运行协程id
    int running_coid_;

public:
    Scheduler(int size);
    ~Scheduler();

    // mainfunc
    void mainfunc();

    // 创建一个协程
    int create_coroutine(Func func, void* arg);

    // 恢复i号协程的执行
    void resume_coroutine(int id);

    // 协程主动让出控制权
    void yield_coroutine();

    // 返回第i号协程的状态
    CoState getstate_coroutine(int id);

    // 返回运行中的协程id
    int get_runningid();

    // 返回协程池的大小,即vector<>大小
    int sizeof_coroutine();

    // 是否完成任务（全部协程）
    bool isfinshed();
};


class Coroutine {
public:
    // 管理此协程的调度器
    Scheduler* sch_;

    // 协程执行的函数以及参数
    Func func_;
    void* arg_;
    
    // 协程的上下文
    ucontext_t context_;
    
    // 协程状态
    enum CoState state_ = RUNNABLE;

    // 协程栈信息, 每个协程独立栈,实际上是位于堆上
    char stack_[STACKSIZE];

public:
    Coroutine(Scheduler* s, Func func, void* arg) 
    : sch_(s), func_(func), arg_(arg) {
        getcontext(&context_);
        context_.uc_link = &s->mainctx_;
        context_.uc_stack.ss_flags = 0;
        context_.uc_stack.ss_sp = stack_;
        context_.uc_stack.ss_size = STACKSIZE;
        makecontext(&context_, reinterpret_cast<void(*)(void)>(&Scheduler::mainfunc), 1, s);
        // 准备好上下文，一旦调用swap()，将执行mainfunc
    }
    // 析构函数
    ~Coroutine() {}
};


Scheduler::Scheduler(int size = 16) : running_coid_(-1) {
    cotines_.reserve(size);
}

Scheduler::~Scheduler(){}

void Scheduler::mainfunc() {
    int id = get_runningid();
    std::shared_ptr<Coroutine> co_ptr = cotines_[id];
    // 协程执行函数
    co_ptr->func_(this, co_ptr->arg_);
    co_ptr->state_ = DEAD;
    running_coid_ = -1;
}

// 创建协程
int Scheduler::create_coroutine(Func func, void* arg) {
    // 找到调度器中空协程，如果有的话
    for (int i = 0; i < cotines_.size(); ++ i) {
        int state = cotines_[i]->state_;
        if (state == DEAD) {     // DEAD，说明该slot可用
            cotines_[i] = std::make_shared<Coroutine>(this, func, arg);
            return i;
        }
    }

    // 没有可用的slot
    cotines_.push_back(std::make_shared<Coroutine>(this, func, arg));
    return cotines_.size() - 1;
}

// resume协程 
void Scheduler::resume_coroutine(int id) {
    int state = getstate_coroutine(id);
    if (state == SUSPEND || RUNNABLE) {     // SUSPEND or RUNNABLE
        std::shared_ptr<Coroutine> co_ptr = cotines_[id];   
        running_coid_ = id;
        co_ptr->state_ = RUNNING;
        swapcontext(&mainctx_, &co_ptr->context_);
    }
}

// yield协程 主动让出CPU执行权
void Scheduler::yield_coroutine() {
    int id = get_runningid();
    assert(id >= 0);
    std::shared_ptr<Coroutine> co_ptr = cotines_[id];
    co_ptr->state_ = SUSPEND;
    running_coid_ = -1;
    // 把当前上下文保存至co_ptr->context_
    // 用mainctx_设置当前上下文，即回到Scheduler
    swapcontext(&co_ptr->context_, &mainctx_);
}

// 返回协程池大小 
int Scheduler::sizeof_coroutine() {
    return cotines_.size();
}


// 返回正在执行任务的协程id
int Scheduler::get_runningid() {
    return running_coid_;
}

// 返回i号协程的状态
CoState Scheduler::getstate_coroutine(int id) {
    assert(id >= 0 && id < cotines_.size());
    if (!cotines_[id])
        return DEAD;
    return cotines_[id]->state_;    // 设置get_state
}

// 判断是否还有协程在执行任务
bool Scheduler::isfinshed() {
    if (running_coid_ != -1) {
        return false;
    } else {
        for (auto ptr : cotines_) {
            if (ptr->state_ != DEAD)
                return false;
        }
    }
    return true;
}
