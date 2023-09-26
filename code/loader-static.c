#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>

#define STK_SZ (1 << 20) // 1 M
#define ROUND(x, align) (void *)(((uintptr_t)x) & ~(align - 1)) // 64 bit，向下取 align 的倍数
#define MOD(x, align) (((uintptr_t)x) & (align - 1))
#define push(sp, T, ...) ({ *((T*)sp) = (T)__VA_ARGS__; sp = (void *)((uintptr_t)(sp) + sizeof(T)); })

void execve_(const char *file, char *argv[], char *envp[])
{
  // WARNING: This execve_ does not free process resources.

  // 开打 .out 文件 file
  int fd = open(file, O_RDONLY);
  assert(fd > 0);

  /* 开始解析 .out 的数据结构 */
  /* 解析依据 RTFM (elf.h) */
  // 把 file 前 4096 个字节映射到内存
  Elf64_Ehdr *h = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(h != (void *)-1);
  assert(h->e_type == ET_EXEC && h->e_machine == EM_X86_64);

  Elf64_Phdr *pht = (Elf64_Phdr *)((char *)h + h->e_phoff);
  for (int i = 0; i < h->e_phnum; i++)
  {
    Elf64_Phdr *p = &pht[i];
    if (p->p_type == PT_LOAD)
    {
      int prot = 0;
      if (p->p_flags & PF_R)
        prot |= PROT_READ;
      if (p->p_flags & PF_W)
        prot |= PROT_WRITE;
      if (p->p_flags & PF_X)
        prot |= PROT_EXEC;
      void *ret = mmap(
          ROUND(p->p_vaddr, p->p_align),              // addr, rounded to ALIGN
          p->p_memsz + MOD(p->p_vaddr, p->p_align),   // length
          prot,                                       // protection
          MAP_PRIVATE | MAP_FIXED,                    // flags, private & strict
          fd,                                         // file descriptor
          (uintptr_t)ROUND(p->p_offset, p->p_align)); // offset
      assert(ret != (void *)-1);
      memset((void *)(p->p_vaddr + p->p_filesz), 0, p->p_memsz - p->p_filesz);
    }
  }
  close(fd); // 自此，所有必需的 code 都加载进了内存

  /* 准备运行前的环境，比如 argv[] envp[] */
  /* 步骤依据 RTFM: System V ABI Figure 3.9 (Initial Process Stack) */
  static char stack[STK_SZ], rnd[16];
  void *sp = ROUND(stack + sizeof(stack) - 4096, 16);
  void *sp_exec = sp;
  int argc = 0;

  // argc
  while (argv[argc])
    argc++;
  push(sp, intptr_t, argc);
  // argv[], NULL-terminate
  for (int i = 0; i <= argc; i++)
    push(sp, intptr_t, argv[i]);
  // envp[], NULL-terminate
  for (; *envp; envp++)
  {
    if (!strchr(*envp, '_')) // remove some verbose ones
      push(sp, intptr_t, *envp);
  }
  // auxv[], AT_NULL-terminate
  push(sp, intptr_t, 0);
  push(sp, Elf64_auxv_t, {.a_type = AT_RANDOM, .a_un.a_val = (uintptr_t)rnd});
  push(sp, Elf64_auxv_t, {.a_type = AT_NULL});

  asm volatile(
      "mov $0, %%rdx;" // required by ABI
      "mov %0, %%rsp;"
      "jmp *%1"
      :
      : "a"(sp_exec), "b"(h->e_entry));
}

int main(int argc, char *argv[], char *envp[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s file [args...]\n", argv[0]);
    exit(1);
  }
  execve_(argv[1], argv + 1, envp);
}
