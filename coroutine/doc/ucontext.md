以下引用部分摘自<ucontext.h>文件，glibc/sysdeps/unix/sysv/linux/x86/sys/<ucontext.h>文件和man手册。

[相关结构体定义在线查看](https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/x86/sys/ucontext.h.html)

用ucontext_t类型的变量来存储上下文信息, glibc提供getcontext, setcontext, makecontext, swapcontext四个函数来对上下文进行操作。

接下来看看ucontext_t变量内部究竟是什么。

```c
typedef struct ucontext_t {
    struct ucontext_t *uc_link;		// 指向后继上下文
    sigset_t          uc_sigmask;	// 阻塞信号集
    stack_t           uc_stack;		// 栈信息
    mcontext_t        uc_mcontext;	// 上下文信息的具体表示，依赖机器平台
    ...
} ucontext_t;
```

`uc_link`指向后继上下文，如果当前上下文是由make_context创建，当当前上下文结束后，后继上下文就会被设置成当前上下文。

`uc_sigmask`是当前上下文的阻塞信号集。

`uc_stack`是当前上下文使用的栈。

```c
// 栈信息
typedef struct {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
} stack_t
```

`uc_mcontext`是当前上下文的具体机器表示，包含调用线程的具体寄存器。

```c
typedef struct {
      /*typedef int greg_t;
        typedef greg_t gregset_t[19]
        gregs是保存寄存器上下文的 
      */
    gregset_t gregs;
    fpregset_t fpregs;
    unsigned long int oldmask;
    unsigned long int cr2;
} mcontext_t;
```

可以看到ucontext_t是对上下文的一个更高层次的封装，具体的上下文信息位于mcontext_t中。

1. getcontext 获取用户上下文并将其设置到 ucp指针指向的ucontext_t类型变量中。

   ```c
    33 /* Get user context and store it in variable pointed to by UCP.  */
    34 extern int getcontext (ucontext_t *__ucp) __THROWNL;
   ```

   >     The function getcontext() initializes the structure pointed at by ucp to the currently active con‐
   >     text.

2. setcontext 通过ucp指针指向的ucontext_t变量设置当前上下文。如果调用成功，将不会返回。

   ```c
    36 /* Set user context from information of variable pointed to by UCP.  */
    37 extern int setcontext (const ucontext_t *__ucp) __THROWNL;
   ```
setcontext()恢复到ucp所指向的上下文, 该上下文必须是由getcontext()或者makecontext()得到,或者作为第三方参数传递给信后处理程序。
   >     The function setcontext() restores the user context pointed at by ucp.  A successful call does not
   >     return.   The  context  should have been obtained by a call of getcontext(), or makecontext(3), or
   >     passed as third argument to a signal handler.
   >     
   >     If the context was obtained by a call of getcontext(), program execution continues as if this call
   >     just returned.
   >     
   >     If  the context was obtained by a call of makecontext(3), program execution continues by a call to
   >     the function func specified as the second argument of that call to makecontext(3).  When the func‐
   >     tion func returns, we continue with the uc_link member of the structure ucp specified as the first
   >     argument of that call to makecontext(3).  When this member is NULL, the thread exits.
   >     
   >     If the context was obtained by a call to a signal handler, then old standard text says that  "pro‐
   >     gram execution continues with the program instruction following the instruction interrupted by the
   >     signal".  However, this sentence was removed in SUSv2, and the present verdict is "the  result  is
   >     unspecified".


3. swapcontext 保存当前上下文至第一个参数ocup指针指向的变量，通过**第二个参数**ucp指针指向变量**设置当前上下文**。

   ```c
    39 /* Save current context in context variable pointed to by OUCP and set
    40    context from variable pointed to by UCP.  */
    41 extern int swapcontext (ucontext_t *__restrict __oucp,
    42             const ucontext_t *__restrict __ucp)
    43   __THROWNL __INDIRECT_RETURN;
   ```

   >     The swapcontext() function saves the current context in the structure pointed to by oucp, and then
   >     activates the context pointed to by ucp.


4. makecontext 修改ucp所指向的上下文变量，当这个ucp指向的上下文变量被恢复时（通过setcontext或者swapcontext）,程序的执行会切换到func去调用。

   ```c
    45 /* Manipulate user context UCP to continue with calling functions FUNC
    46    and the ARGC-1 parameters following ARGC when the context is used
    47    the next time in `setcontext' or `swapcontext'.
    48
    49    We cannot say anything about the parameters FUNC takes; `void'
    50    is as good as any other choice.  */
    51 extern void makecontext (ucontext_t *__ucp, void (*__func) (void),
    52              int __argc, ...) __THROW;
   ```
makecontext()修改ucp指向的上下文（ucp从getcontext获取），在调用makecontext之前，调用者必须为该上下文分配一个new stack，设置ucp->stack，然后为其指定后继上下文，赋值给ucp->uc_link。
   
   如果被设置的上下文后来被激活了（通过setcontext()或者swapcontext()），func就会被调用，func调用结束返回后，后继上下文即ucp->uc_link会被激活，如果uc_link为NULL，线程结束退出。
   
   >     The  makecontext() function modifies the context pointed to by ucp (which was obtained from a call
   >     to getcontext(3)).  Before invoking makecontext(), the caller must allocate a new stack  for  this
   >     context and assign its address to ucp->uc_stack, and define a successor context and assign its ad‐
   >     dress to ucp->uc_link.
   >
   >     When this context is later activated (using setcontext(3) or swapcontext()) the function  func  is called, and passed the series of integer (int) arguments that follow argc; the caller must specify
   >     the number of these arguments in argc.  When this function returns, the successor context is acti‐
   >     vated.  If the successor context pointer is NULL, the thread exits.

   