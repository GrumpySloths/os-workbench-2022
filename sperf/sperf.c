#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include<regex.h>

int main(int argc, char *argv[]) {
  char *exec_argv[] = { "strace","-T","ls", NULL, };
  char *exec_envp[] = {  "PATH=/bin",NULL, };
  int pipefd[2];
  pid_t cpid;
  char buf;

  if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
  }

  cpid = fork();
  if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
  }

  if (cpid != 0) {      //parent read pipe
      close(pipefd[1]);          /* Close unused write end */

      // while (read(pipefd[0], &buf, 1) > 0)
      //     write(STDOUT_FILENO, &buf, 1);

      write(STDOUT_FILENO, "\n", 1);
      close(pipefd[0]);
      _exit(EXIT_SUCCESS);

  } else {            /* child write pipe */
      close(pipefd[0]);          /* Close unused read end */
      printf("pipedf[1]:%d\n", pipefd[1]);
      // dup2(pipefd[1], 1);
      close(1);
      execve("/bin/strace", exec_argv, exec_envp);
      perror(argv[0]);
      exit(EXIT_FAILURE);
  }
  // execve("strace",          exec_argv, exec_envp);

  // execve("/usr/bin/strace", exec_argv, exec_envp);

}
