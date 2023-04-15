#include <stdio.h>
#include <unistd.h>
#include <ucontext.h>

int main(void)
{
    ucontext_t context;

    getcontext(&context);
    sleep(5);
    printf("Hello world\n");
    
    setcontext(&context);
    return 0;
}
