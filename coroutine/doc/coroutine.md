协程至少应满足的语义

yield语义，让出CPU执行权；resume语义，恢复执行权。

### 对称式协程和非对称式协程

对称式协程：对称式协程，不论yield还是resume都可以把控制权交由任意其它coroutine。

非对称式协程：非对称式协程的yield只能把控制权交给当前coroutine的caller（说resumer更合适——resume了当前coroutine）。

### 基于ucontext实现自己的协程库。（非对称式协程）

至少满足的功能需求：用户可以自行创建一个（多个）协程，可以切换到某个协程执行操作，协程可以让出控制权回到resumer。

#### 协程对象Coroutine

用Coroutine类来表示协程，同时需要一个调度器Scheduler来管理协程。我们实现的是非对称式协程，在这里resumer即是调度器Scheduler，每个协程yield后执行权都会回到Scheduler中。

一个Coroutine对象应包含如下信息：

1. 当前协程执行的函数

2. 当前协程的上下文环境

3. 当前协程栈信息 （用来设置ucontext_t）

4. 当前协程的状态（方便协程的调度）

```cpp
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
	......
}
enum CoState {
    DEAD,
    RUNNABLE,   
    RUNNING,
    SUSPEND
};
```

#### 协程管理器

协程的管理都通过一个协程管理器Scheduler来协调，创建协程，切换协程。使用容器vector来存储协程对象，这里也是一种池化思想，不必每次都创建销毁协程对象。Scheduler提供三个接口来创建协程，恢复协程，协程让出：

1. int create_coroutine(Func func, void* arg); 

   在创建协程时，应通过getcontext() + make_context()构造好协程对象的上下文，后续resume时或者yield时，通过swap_context()进行执行权的转换。

2. void resume_coroutine(int id); 

   恢复协程，即将控制权转到待恢复的协程，这里resume语义包括“初始启动”的含义，即协程初始执行任务的功能也由resume_coroutine来完成。

   主要进行的动作：swapcontext(\&mainctx\_, &co\_ptr->context\_); 即将当前上下文（Scheduler上下文）保存，将协程对象的上下文置成当前上下文。

3. void yield_coroutine(); 

   协程让出CPU控制权，控制权回到resumer。

   主要进行的动作：  swapcontext(&co\_ptr->context\_, &mainctx\_); 把当前上下文保存至协程对象中，回到Scheduler。

```cpp
class Scheduler {
public:
    // 主上下文
    ucontext_t mainctx_;
    
    // 协程数组
    std::vector<std::shared_ptr<Coroutine>> cotines_;
	......
public:
    // 创建一个协程
    int create_coroutine(Func func, void* arg);

    // 恢复i号协程的执行
    void resume_coroutine(int id);

    // 协程主动让出控制权
    void yield_coroutine();
	......
};
```

代码地址：

### 使用示例

创建两个协程，运行协程，协程中可通过主动进行yield操作。

```cpp
#include <memory>
#include <cstdio>
#include "Coroutine.hpp"

void foo(Scheduler *s, void *para) {
    int *arg = (int *)para;
    for(int i = 0; i < 5; i++) {
        printf("Coroutine %d : %d \n", s->get_runningid(), *arg + i);
        s->yield_coroutine();
    }
}
int main() {
    std::shared_ptr<Scheduler> s = std::make_shared<Scheduler>();
    int a = 0;
    int b = 1000;
    int co1id = s->create_coroutine(foo, &a);
    int co2id = s->create_coroutine(foo, &b);
    
    printf("main start.\n");

    while (!s->isfinshed()) {
        s->resume_coroutine(co1id);
        s->resume_coroutine(co2id);
        // 轮流输出
        // 0 -> 100 -> 1 -> 101 -> 2 -> 102 -> 3 -> 103 ...
    }

    printf("main end.\n");

    return 0;
}
```

