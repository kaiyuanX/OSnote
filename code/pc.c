/* 生产者消费者问题 */

#include "thread.h"
#include "thread-sync.h"

int n, count = 0;
mutex_t lk = MUTEX_INIT();

void Tproduce(int id)
{
  while (1)
  {
  retry:
    mutex_lock(&lk);
    if (count == n)
    {
      mutex_unlock(&lk);
      goto retry;
    }
    count++;

    // printf("(");

    FILE *x = fopen("out.txt", "a");
    // fprintf(x, "id = %d (\n", id);
    fprintf(x, "(");
    fclose(x);

    mutex_unlock(&lk);
    // sleep(1);
  }
}

void Tconsume(int id)
{
  while (1)
  {
  retry:
    mutex_lock(&lk);
    if (count == 0)
    {
      mutex_unlock(&lk);
      goto retry;
    }
    count--;

    // printf(")");

    FILE *x = fopen("out.txt", "a");
    // fprintf(x, "id = %d )\n", id);
    fprintf(x, ")");
    fclose(x);

    mutex_unlock(&lk);
    // sleep(1);
  }
}

int main(int argc, char *argv[])
{
  assert(argc == 2);
  n = atoi(argv[1]);
  setbuf(stdout, NULL);

  for (int i = 0; i < 8; i++)
  {
    create(Tproduce);
    create(Tconsume);
  }
}
