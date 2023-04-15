## 项目简介

- 基于*nix的`<ucontext.h>`由C++11实现的**轻量级协程库**。

- 采用**非对称式(Asymmetric)模式。**

- 用户通过协程调度器Scheduler创建管理协程，协程可主动yield让出CPU执行权。

## 技术要点

1. 通过ucontext保存上下文环境，利用<ucontex.h>组件进行上下文切换。

2. 每个协程的栈独立，构造在堆上，默认大小为8K。

3. 通过std::vector管理协程，协程池大小可自动扩容。

## 开发工具

gcc (Ubuntu 9.3.0-10ubuntu2) 9.3.0

vim 8.1.2269

GNU gdb (Ubuntu 9.1-0ubuntu1) 9.1

## 测试

测试程序：运行`bin/performance_test`程序。

测试平台：1核 2GB云服务器  Linux VM-0-4-ubuntu 4.15.0-118-generic

测试结果：每秒约能进行8e5次切换。

```shell
TEST START
Finshed
Switch 801077times in a second.
```

## API

协程调度器的成员函数：  

- `int create_coroutine(Func func, void* arg);`	创建一个协程
- `void resume_coroutine(int id);`	恢复i号协程的执行
- `void yield_coroutine();`	协程主动让出控制权
- `CoState getstate_coroutine(int id);`	返回第i号协程的状态
- `int get_runningid();`	返回运行中的协程id
- `int sizeof_coroutine();`  返回协程池的大小  
- `bool isfinshed();`      所有协程是否完成任务

## 使用示例

- 示例程序

```cpp
#include <memory>
#include <cstdio>
#include "Coroutine.hpp"

void foo(Scheduler *s, void *para) {
    int* arg = static_cast<int*>(para);
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
    }
    printf("main end.\n");
    return 0;
}
```

- 输出结果：

  ```shell
  main start.
  Coroutine 0 : 0
  Coroutine 1 : 1000
  Coroutine 0 : 1
  Coroutine 1 : 1001
  Coroutine 0 : 2
  Coroutine 1 : 1002
  Coroutine 0 : 3
  Coroutine 1 : 1003
  Coroutine 0 : 4
  Coroutine 1 : 1004
  main end.
  ```

## 未来可能的改进

- 添加对对称式(Symmetrica)协程的支持