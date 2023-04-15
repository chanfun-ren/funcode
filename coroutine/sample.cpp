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
        // 0 -> 100 -> 1 -> 101 -> 2 -> 102 -> 3 -> 103 ...
    }
    printf("main end.\n");
    return 0;
}