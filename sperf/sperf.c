#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc;i++){
        printf("%s\n", argv[i]);
    }
        char *exec_argv[] = {
            "strace",
            "ls",
            NULL,
        };
    char *exec_envp[] = {
        "PATH=/bin",
        NULL,
    };
    execve("strace", exec_argv, exec_envp);
    execve("/bin/strace", exec_argv, exec_envp);
    execve("/usr/bin/strace", exec_argv, exec_envp);
    perror(argv[0]);
    exit(EXIT_FAILURE);
}
