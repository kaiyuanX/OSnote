#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

typedef struct pidinfo
{
    char name[50];
    __pid_t pid;
    __pid_t ppid;
} PidInfo;

// 所有进程的信息记录在这里
PidInfo pidinfos[10000];
int pid_count = 0;

// get commandline ops
char *readcmdops(int argc, char *argv[])
{
    assert(argc >= 1);
    if (argc == 1) // no ops
    {
        return NULL;
    }
    char *p = argv[1];
    char ops[10];
    int j = 0;
    for (int i = 0; i < strlen(p); i++)
    {
        if (p[i] == '-')
        {
            continue;
        }
        ops[j] = p[i];
        j++;
    }
    strcpy(p, ops);
    return p;
}

pid_t readprocessname_ppid(__pid_t pid, char name[])
{
    char *str = (char *)malloc(sizeof(char) * 20);
    if (str == NULL)
    {
        printf("malloc failed");
        exit(1);
    }

    sprintf(str, "%d", pid);
    char processpath[20] = "/proc/";
    strcat(processpath, str);
    strcat(processpath, "/stat");

    // processpath == /proc/[num]/stat
    FILE *fp = fopen(processpath, "r");
    if (fp)
    {
        char i;
        __pid_t _pid, ppid = 0; // ppid 初始化为 0，就不会打印出奇怪的结果
        char pname[40];

        fscanf(fp, "%d (%s %c %d", &_pid, pname, &i, &ppid); // 进程信息在 /proc/[num]/stat 文件中的格式
        pname[strlen(pname) - 1] = '\0';
        strcpy(name, pname);
        assert(pid == _pid);
        fclose(fp);
        free(str);
        return ppid;
    }
    else
    {
        printf("open failed");
        free(str);
        exit(1);
    }
}

void setProcessInfo()
{
    DIR *dp = opendir("/proc");

    // 遍历 /proc 目录下的文件
    if (!dp)
    {
        fprintf(stderr, "%s", "Can 't open /proc/");
        exit(1);
    }
    else
    {
        int pid = 0;
        struct dirent *entry;
        while ((entry = readdir(dp)) != NULL)
        {
            if ((pid = atoi(entry->d_name)) == 0)
            {
                continue;
            }
            else
            { // store in pidinfo (struct pidinfo{})
                pidinfos[pid_count].pid = pid;
                pidinfos[pid_count].ppid = readprocessname_ppid(pid, pidinfos[pid_count].name);
                assert(pidinfos[pid_count].ppid > -1);
                // printf("%d (%s) %d\n",pidinfos[pid_count].pid,pidinfos[pid_count].name,pidinfos[pid_count].ppid);
                pid_count++;
            }
        }
    }
}

typedef struct processtree
{
    pid_t pid;
    char name[40];
    struct childprocesses *children;
} Processtree;

struct childprocesses
{
    struct childprocesses *next; // x 的多个子进程用 next 连接
    Processtree *proc;
};

// 把 pid 的所有子进程 index 存进 index[]
void findallchildrens(int pid, int index[])
{
    int indexcount = 0;
    size_t i = 0;
    for (; i < pid_count - 1; i += 2)
    {
        if (pidinfos[i].ppid == pid)
        {
            index[indexcount++] = i;
        }
        if (pidinfos[i + 1].ppid == pid)
        {
            index[indexcount++] = i + 1;
        }
    }
    for (; i < pid_count; i++)
    {
        if (pidinfos[i].ppid == pid)
        {
            index[indexcount++] = i;
        }
    }
}

// 跟 creat_tree_nopid() 并无本质区别
void creat_tree(Processtree *root, int tab_length)
{
    int flag = 2;
    char str[100] = {0};
    int allchildrensindex[500] = {0}; // Log the index of all root child processes in Pidinfos
    findallchildrens(root->pid, allchildrensindex);
    root->children = (struct childprocesses *)malloc(sizeof(struct childprocesses));
    if (root->children == NULL)
    {
        printf("malloc failed");
        exit(1);
    }
    struct childprocesses *child = root->children;
    if (allchildrensindex[0] == 0) // it's leaf node,no child,return
    {
        printf("%s(%d)", root->name, root->pid);
        return;
    }
    sprintf(str, "%s(%d)-", root->name, root->pid);
    printf("%s", str);
    if (allchildrensindex[1] != 0)
    {
        printf("+-");
    }
    else
    {
        flag = 0;
    }
    for (int i = 0; i < 500 && allchildrensindex[i] != 0; i++) // Traverses all children of the process
    {
        child->proc = (Processtree *)malloc(sizeof(Processtree));
        if (child->proc == NULL)
        {
            printf("malloc failed");
            exit(1);
        }
        child->proc->pid = pidinfos[allchildrensindex[i]].pid;
        strcpy(child->proc->name, pidinfos[allchildrensindex[i]].name);
        creat_tree(child->proc, strlen(str) + tab_length + flag); // recursive
        if (i + 1 < 500 && allchildrensindex[i + 1] != 0)
        {
            child->next = (struct childprocesses *)malloc(sizeof(struct childprocesses));
            if (child->next == NULL)
            {
                printf("malloc failed");
                exit(1);
            }
            child = child->next;
            char tabs[100];
            printf("\n");
            for (size_t i = 0; i < strlen(str) + tab_length; i++)
            {
                printf(" ");
            }
            printf("|-");
        }
    }
    free(root);
}

// print root 节点的儿子进程
void creat_tree_nopid(Processtree *root, int tab_length)
{
    int flag = 2;
    char str[100] = {0};
    int allchildrensindex[500] = {0}; // 假设每个进程的子进程不超过 500 个

    // 得到 root->pid 所有的子进程位置，存在 allchildrensindex
    findallchildrens(root->pid, allchildrensindex);

    root->children = (struct childprocesses *)malloc(sizeof(struct childprocesses));
    if (root->children == NULL)
    {
        printf("malloc failed");
        exit(1);
    }
    struct childprocesses *child = root->children;

    // 没儿子
    if (allchildrensindex[0] == 0)
    {
        printf("%s", root->name); // TODO 缺陷列表2  leafnode task:: /proc/pid/task/.../stat
        return;
    }

    // 有一个儿子
    sprintf(str, "%s-", root->name);
    printf("%s", str);

    // 有两个儿子
    if (allchildrensindex[1] != 0)
    {
        printf("+-");
    }
    else
    {
        flag = 0;
    }

    // Traverses all children of the process
    for (int i = 0; i < 500 && allchildrensindex[i] != 0; i++)
    {
        child->proc = (Processtree *)malloc(sizeof(Processtree));
        if (child->proc == NULL)
        {
            printf("malloc failed");
            exit(1);
        }

        child->proc->pid = pidinfos[allchildrensindex[i]].pid;
        strcpy(child->proc->name, pidinfos[allchildrensindex[i]].name);
        creat_tree_nopid(child->proc, strlen(str) + tab_length + flag); // recursive

        if (i + 1 < 500 && allchildrensindex[i + 1] != 0)
        {
            child->next = (struct childprocesses *)malloc(sizeof(struct childprocesses));
            if (child->next == NULL)
            {
                printf("malloc failed");
                exit(1);
            }
            child = child->next;
            char tabs[100];
            printf("\n");
            for (size_t i = 0; i < strlen(str) + tab_length; i++)
            {
                printf(" ");
            }
            printf("|-");
        }
    }
    free(root);
}

int main(int argc, char *argv[])
{
    char *ops = readcmdops(argc, argv);

    if (!ops) // 没参数
    {
        setProcessInfo();
        Processtree *root = (Processtree *)malloc(sizeof(Processtree));
        root->pid = pidinfos[0].pid;
        strcpy(root->name, pidinfos[0].name);
        creat_tree_nopid(root, 0);
    }
    else
    {
        if (!strcmp(ops, "p")) // ops = -p
        {
            // printf("%s",ops);
            setProcessInfo();
            Processtree *root = (Processtree *)malloc(sizeof(Processtree));
            root->pid = pidinfos[0].pid;
            strcpy(root->name, pidinfos[0].name);
            creat_tree(root, 0);
        }
        else if (!strcmp(ops, "n")) //  -n
        {
            // printf("%s",ops);
            setProcessInfo();
            Processtree *root = (Processtree *)malloc(sizeof(Processtree));
            root->pid = pidinfos[0].pid;
            strcpy(root->name, pidinfos[0].name);
            creat_tree_nopid(root, 0);
        }
        else if (!strcmp(ops, "V")) //    -V
        {
            printf("pstree (PSmisc) UNKNOWN\nCopyright (C) 1993-2019 Werner Almesberger and Craig Small\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.");
        }
        else
        {
            printf("%s", "wrong ops");
            return -1;
        }
    }
    return 0;
}