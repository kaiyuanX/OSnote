#include "thread.h"
#include "thread-sync.h"

int n, count = 0;
mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();
cond_t cv2 = COND_INIT();

void Tproduce()
{
  while (1)
  {
    mutex_lock(&lk);

    while (count == n)
    {
      cond_wait(&cv, &lk);
    }

    // assert(count != n);
    // printf("(");
    FILE *x = fopen("out.txt", "a");
    // fprintf(x, "id = %d (\n", id);
    fprintf(x, "(");
    fclose(x);
    count++;

    cond_signal(&cv2);
    // cond_broadcast(&cv);
    mutex_unlock(&lk);
  }
}

void Tconsume()
{
  while (1)
  {
    mutex_lock(&lk);
    while (count == 0)
    {
      pthread_cond_wait(&cv2, &lk);
    }
    // assert(count != 0);
    // printf(")");
    FILE *x = fopen("out.txt", "a");
    // fprintf(x, "id = %d (\n", id);
    fprintf(x, ")");
    fclose(x);
    count--;
    cond_signal(&cv);
    // cond_broadcast(&cv);
    mutex_unlock(&lk);
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
