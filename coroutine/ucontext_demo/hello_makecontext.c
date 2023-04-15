#include <stdio.h>
#include <unistd.h>
#include <ucontext.h>

void func() {
    printf("print func.\n");
}

int main() {
    ucontext_t context1;
    char stack[1024];

    getcontext(&context1);
    
    context1.uc_stack.ss_sp = stack;
    context1.uc_stack.ss_size = sizeof(stack);
    context1.uc_link = NULL;
    
    makecontext(&context1, func, 0);

    printf("hello, world.\n");

    setcontext(&context1);

    return 0;

}
