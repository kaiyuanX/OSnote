#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  // exec_argv
  char **exec_argv = (void *)malloc(sizeof(void *) * (argc + 2));
  exec_argv[0] = "stace";
  exec_argv[1] = "-T";
  for (int i = 2; i < argc + 1; i++)
  {
    exec_argv[i] = argv[i - 1];
  }
  exec_argv[argc + 1] = NULL;

  // exec_envp
  char *exec_envp[] = {
      "PATH=/bin",
      NULL,
  };

  // pipe
  // 所有的文件描述符都会在 fork 的时候被继承
  int fildes[2];
  if (pipe(fildes) != 0)
  {
    // 出错处理
    printf("get pipe error!\n");
    return 0;
  }

  // fork
  int pid = fork();
  if (pid == 0)
  {
    // 子进程，执行 strace 命令
    dup2(fildes[1], 2);
    execve("/bin/strace", exec_argv, exec_envp);
    // 不应该执行下面代码，否则 execve 失败，出错处理
    printf("execve error!\n");
    return 0;
  }
  else
  {
    sleep(3);
    fprintf(stdout, "read start! \n");
    fprintf(stdout, "%d %d \n", fildes[0], fildes[1]);
    fflush(stdout);

    // 父进程，读取 strace 输出并统计
    while (1)
    {
      char output[1000] = "echo '"; // "' >> a.txt"
      int i = 6;

      while (1)
      {
        char tmp[10];

        read(fildes[0], &tmp, 1); // fildes[0] is a pipe, when can i break 'while' ?

        if (tmp[0] == '\n')
        {
          output[i++] = '\'';
          output[i++] = ' ';
          output[i++] = '>';
          output[i++] = '>';
          output[i++] = ' ';
          output[i++] = 'a';
          output[i++] = '.';
          output[i++] = 't';
          output[i++] = 'x';
          output[i] = 't';

          system(output);
          break;
        }

        output[i] = tmp[0];
        i++;
      }
    }

    /* 自此父进程能正确拿到 strace 的输出，只差最后一步 “解析 strace 的输出” */
    /* to be... */
  }

  // // test get input   checked!
  //
  // printf("%d\n",argc);
  // for (int i = 0; i < argc + 1; i++)
  // {
  //   printf("%s\n", exec_argv[i]);
  // }

  // // execve test
  //
  // execve("strace", exec_argv, exec_envp);      // in my machine, fail this line
  // execve("/bin/strace", exec_argv, exec_envp); // success !
  // execve("/usr/bin/strace", exec_argv, exec_envp);
  // perror(argv[0]);
  // exit(EXIT_FAILURE);
}
