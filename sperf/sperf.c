#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include<regex.h>

#define MAX_LINE 512

int readline(int fd, char *buffer, int max_length) {
    int bytes_read = 0;
    int bytes_read_total = 0;
    char ch;

    while (bytes_read_total < max_length - 1) {
        if (read(fd, &ch, 1) == 1) {
            buffer[bytes_read_total++] = ch;
            if (ch == '\n') {
                break;
            }
        } else {
            break;
        }
    }

    buffer[bytes_read_total] = '\0';

    return bytes_read_total;
}


int main(int argc, char *argv[]) {
  char *exec_argv[] = { "strace","-T","ls", NULL, };
  char *exec_envp[] = {  "PATH=/bin",NULL, };
  int pipefd[2];
  pid_t cpid;
  char buffer[MAX_LINE];
  char buf;

  regex_t regex_1;
  regex_t regex_2;
  regmatch_t matches1[2];
  regmatch_t matches2[2];
  char *pattern_1 = "([^\\(]+)\\(";  // 匹配"("前的内容
  char *pattern_2 = "<([^>]*)>"; // 匹配尖括号<>内的内容
  if (regcomp(&regex_1, pattern_1, REG_EXTENDED) != 0) {
      fprintf(stderr, "Failed to compile regex_1\n");
      return 1;
  }
  if (regcomp(&regex_2, pattern_2, REG_EXTENDED) != 0) {
      fprintf(stderr, "Failed to compile regex_2\n");
      return 1;
  }
  
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
      while (readline(pipefd[0],buffer,MAX_LINE) > 0){
        if(strncmp(buffer,"exit_group",strlen("exit_group"))==0)
            break;
        // "("前内容匹配
        if (regexec(&regex_1, buffer, 2, matches1, 0) == 0) {
            for (size_t i = matches1[1].rm_so; i < matches1[1].rm_eo; i++) {
                putchar(buffer[i]);
            }
            putchar(' ');
        } else {
            printf("No match for reg_1\n");
        }
        //<>内内容匹配
        if (regexec(&regex_2, buffer, 2, matches2, 0) == 0) {
            for (size_t i = matches2[1].rm_so; i < matches2[1].rm_eo; i++) {
                putchar(buffer[i]);
            }
            putchar('\n');
        } else {
            printf("No match for reg_2\n");
        }
      }
      write(STDOUT_FILENO, "\n", 1);
      close(pipefd[0]);
      regfree(&regex_1);
      regfree(&regex_2);
      _exit(EXIT_SUCCESS);

  } else {            /* child write pipe */
      close(pipefd[0]);          /* Close unused read end */
      dup2(pipefd[1], 2);
      execve("/bin/strace", exec_argv, exec_envp);
      perror(argv[0]);
      exit(EXIT_FAILURE);
  }
  // execve("strace",          exec_argv, exec_envp);

  // execve("/usr/bin/strace", exec_argv, exec_envp);

}
