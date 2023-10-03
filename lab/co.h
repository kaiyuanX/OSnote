// coroutine

// 创建一个新的协程
//      新创建的协程从函数 func 开始执行，并传入参数 arg
//      新创建的协程不会立即执行，而是调用 co_start 的协程继续执行
//      使用协程的 struct co 的具体定义留在 co.c 中；自由发挥
//      co_start 返回的 struct co 指针需要分配内存。我们推荐使用 malloc() 分配
struct co *co_start(const char *name, void (*func)(void *), void *arg);

// 实现协程的切换
//      协程运行后一直在 CPU 上执行，直到 func 函数返回或调用 co_yield() 使当前运行的协程暂时放弃执行
//      co_yield() 时若系统中有多个可运行的协程时 (包括当前协程)，你应当随机选择下一个系统中可运行的协程
void co_yield ();

// 表示调用者需要等待，直到 co 协程的执行完成才能继续执行
//      co_wait() 返回前，应该 free(co)
void co_wait(struct co *co);


// main 函数的执行也是一个协程，因此可以在 main 中调用 co_yield 或 co_wait
// main 函数返回后，无论有多少协程，进程都将直接终止
