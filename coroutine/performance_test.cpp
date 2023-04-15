#include <iostream>
#include <memory>
#include "Coroutine.hpp"
#include <unistd.h>
#include <signal.h>

long long switch_times = 0;

void handler(int ) {
    std::cout << "Finshed\n";
    std::cout << "Switch " << switch_times << "times in a second.\n";
    exit(0);
}

void func(Scheduler* s, void* arg) {
    while (1) {
        s->yield_coroutine();
    }
}

int main() {
    std::shared_ptr<Scheduler> s = std::make_shared<Scheduler>();
    int coid1 = s->create_coroutine(func, nullptr);
    int coid2 = s->create_coroutine(func, nullptr);

    std::cout << "TEST START\n";
    signal(SIGALRM, handler);
    alarm(1);
    while (!s->isfinshed()) {
        s->resume_coroutine(coid1);
        ++ switch_times;
        s->resume_coroutine(coid2);
        ++ switch_times;
    }

    return 0;
}