/* crepl */

// one thing
//      输入不合法的 C 代码，程序应该给出错误提示而不应该 crash
// 这一点我并没有实现
// 大概思路是在 fprintf 前检查语法错误

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

char template[] = "/tmp/mycreplXXXXXX"; // char [18] exclude '\0'
int expCount = 0;

// line start with "int " return ture
int IsFunc(char *line, int lineLen)
{
    int i = 0;

    while (line[i] == ' ')
    {
        i++;
    }

    if ((i + 3) <= lineLen && line[i] == 'i' && line[i + 1] == 'n' && line[i + 2] == 't' && line[i + 3] == ' ')
    {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    static char line[1000];

    int fd = mkstemp(template);
    if (fd == -1)
        printf("mkstemp");
    close(fd);
    char renameIt[50];
    sprintf(renameIt, "mv %s %s.c", template, template);
    system(renameIt);

    char fileNameC[24];
    strcpy(fileNameC, template);
    sprintf(fileNameC, "%s.c", template);
    char fileNameSo[24];
    sprintf(fileNameSo, "%s.so", template);

    void *handle = NULL;

    while (1)
    {
        printf("crepl> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
        {
            break;
        }

        // printf("Got %zu chars.\n", strlen(line));

        // type exit to quit
        if (!strcmp(line, "exit\n"))
        {
            printf("user exit!\n");
            system("rm /tmp/mycrepl*");
            return 0;
        }

        int lineLen = strlen(line);

        if (lineLen > 3 && IsFunc(line, lineLen)) // is a func
        {
            // 解析 func 的参数列表
            // too lazy too make

            //
            FILE *fp = fopen(fileNameC, "a");
            if (fp == NULL)
            {
                printf("open template.c fail\n");
                return 0;
            }
            fprintf(fp, "%s\n", line);
            fclose(fp);
        }
        else // is a exp
        {
            // expName
            char expName[30];
            sprintf(expName, "%s_%d", template + 5, expCount);
            expCount++;

            //
            FILE *fp = fopen(fileNameC, "a");
            if (fp == NULL)
            {
                printf("open template.c fail\n");
                return 0;
            }
            line[lineLen - 1] = '\0';
            fprintf(fp, "int %s ()\n{\n    return (%s);\n}\n\n", expName, line);
            fclose(fp);

            // gcc
            char gccIt[70];
            sprintf(gccIt, "gcc -shared -fpic -o %s.so %s.c", template, template);
            system(gccIt);

            //
            if (handle != NULL)
            {
                dlclose(handle);
            }
            handle = dlopen(fileNameSo, RTLD_LAZY);
            int (*g)(void);
            g = dlsym(handle, expName);
            if (g == NULL)
            {
                printf("load g() fail!\n");
                return 0;
            }
            printf("crepl> result = %d \n", g());
        }
    }
}
