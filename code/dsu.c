#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>

#define PG_SIZE 4096

// 结构体记录 jmp 操作的 opcode 和 des
struct jmp
{
    uint32_t opcode : 8;
    int32_t offset : 32;
} __attribute__((packed));

// 创建一个结构体，记录 jmp 指令
// 见手册 e9 为 相对跳转
#define JMP(off) ((struct jmp){0xe9, off - sizeof(struct jmp)}) // sizeof(struct jmp) == 8

static inline bool within_page(void *addr)
{
    return (uintptr_t)addr % PG_SIZE + sizeof(struct jmp) <= PG_SIZE;
}

void DSU(void *old, void *new)
{
    void *base = (void *)((uintptr_t)old & ~(PG_SIZE - 1)); // base 为 4096 的整数倍 （ mprotect 的要求 ）

    size_t len = PG_SIZE * (within_page(old) ? 1 : 2);

    int flags = PROT_WRITE | PROT_READ | PROT_EXEC;
    if (mprotect(base, len, flags) == 0)
    {
        // 它把旧函数的第一条指令 patch 成一条向新版本函数的跳转指令
        *(struct jmp *)old = JMP((char *)new - (char *)old); // **PATCH**
        mprotect(base, len, flags & ~PROT_WRITE);
    }
    else
    {
        perror("DSU fail");
    }
}

void foo() { printf("In %s();\n", __func__); }
void foo_new() { printf("In updated %s();\n", __func__); }

int main()
{
    foo();
    DSU(foo, foo_new);
    foo();
}