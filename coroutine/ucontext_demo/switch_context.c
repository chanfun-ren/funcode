#include <stdio.h>
#include <unistd.h>
#include <ucontext.h>

void func(void *arg) {
    printf("I'm func.\n");
}


int main() {
    char stack[1024*128];
    ucontext_t child, parent;
    
    getcontext(&child);
    child.uc_stack.ss_sp = stack;
    child.uc_stack.ss_size = sizeof(stack);
    child.uc_stack.ss_flags = 0;
    child.uc_link = &parent;

    makecontext(&child, (void(*)(void))(func), 0);

    swapcontext(&parent, &child);

    printf("I'm parent.\n");
    return 0;
}
