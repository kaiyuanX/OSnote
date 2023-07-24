#include <stdio.h>
#define N 3

int main()
{
    FILE *x = fopen("out.txt", "r");

    if (x == NULL)
    {
        printf("nofile");
    }
    int count = 0;
    char d = '1';
    int times = 0;

    while (1 == fscanf(x, "%c", &d))
    {
        times++;
        if (d == '(')
        {
            count++;
            if (count > 5)
            {
                printf("times = %d count = %d, worry!", times, count);
                break;
            }
        }
        else if (d == ')')
        {
            count--;
            if (count < 0)
            {
                printf("times = %d count = %d, worry!", times, count);
                break;
            }
        }
        d = '1';
    }

    fclose(x);

    printf("done %d", count);
}