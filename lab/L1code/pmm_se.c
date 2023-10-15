/* 速通版的代码尝试 */
//
// bugs
// 1. test 中有时候会在 heap full 之前就突然结束，没有任何报错，不知道原因？
// 2. line 45 when case: 'heap full' happen
//      - halt() panic_on() assert()
//      - 会导致一些 ERROR，不知道为什么？

// 下一个分配的位置
static char *pos;

#define atomic \
  for (int __i = (lock(), 0); __i < 1; __i++, unlock())

/* 自旋锁 */
static int locked = 0;
static void lock()
{
  while (atomic_xchg(&locked, 1))
    ;
}
static void unlock() { atomic_xchg(&locked, 0); }
/* end */

// my work
static void *kalloc(size_t size) 
{
  char *ret;

  int sz = 1;
  while (sz < size)
  {
    sz *= 2;
  }

  atomic
  {
    while ((intptr_t)pos % sz != 0) // 对齐要求
    {
      pos++;
    }
    ret = pos;
    pos += sz;

    // test info
    assert(locked == 1);
    if (pos > (char *)heap.end) // case: heap full
    {
      // 使用 panic_on(pos > (char *)heap.end, "full"); 或者 assert() 会导致一些 ERROR，不知道为什么？
      // 好吧，使用 halt 依然会 ERROR
      halt(1);
    }
    printf("on CPU %d , ret: %p, alloc: %d to %d \n", cpu_current(), ret, size, sz);
  }

  return ret;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  // my work
  pos = heap.start;
}
