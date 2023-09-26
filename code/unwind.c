#include <stdio.h>
#include <stdlib.h>

const char *binary;

struct frame
{
  struct frame *next; // old rbp
  void *addr;         // ret addr
};

void backtrace()
{
  struct frame *f;
  char cmd[1024];
  extern char end; // end 的地址似乎在本组调用的栈位置最上

  asm volatile("movq %%rbp, %0"
               : "=g"(f));
  for (; f->addr < (void *)&end; f = f->next)
  {
    printf("%016lx  ", (long)f->addr);
    fflush(stdout);
    sprintf(cmd, "addr2line -e %s %p", binary, f->addr);
    system(cmd);
  }
}

void bar()
{
  backtrace();
}

void foo()
{
  bar();
}

int main(int argc, char *argv[])
{
  binary = argv[0];
  foo();
}
