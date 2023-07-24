#include "thread.h"
#include "thread-sync.h"

/* 参数传入总位置 N */
/* fill 表示已有资源的个数 f */
/* empty 表示空位的个数 e */
/* f + e = N */
sem_t fill, empty;

void producer()
{
  while (1)
  {
    P(&empty);
    printf("(");
    V(&fill);
  }
}

void consumer()
{
  while (1)
  {
    P(&fill);
    printf(")");
    V(&empty);
  }
}

int main(int argc, char *argv[])
{
  assert(argc == 2);

  SEM_INIT(&fill, 0);
  SEM_INIT(&empty, atoi(argv[1]));

  for (int i = 0; i < 8; i++)
  {
    create(producer);
    create(consumer);
  }
}
