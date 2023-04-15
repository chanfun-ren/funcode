#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

void func(char* str) {
    printf("%s", str);
}

int main() {
    ucontext_t context1;
    
    getcontext(&context1);
    
    printf("hello, ucontext\n");
    sleep(1);
    
    setcontext(&context1);

    return 0;
}
