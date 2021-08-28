// Taken from https://stackoverflow.com/a/3643188/4632735
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int
main(int argc, char* argv[])
{
    pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork();

    if(pid==0)
    {
        printf("i'm the child used for ls \n");
        int fd1 = dup2(fd[WRITE_END], STDOUT_FILENO);
        close(fd[WRITE_END]);
        execlp("ls", "ls", "-al", NULL);
    }
    else
    {
        pid=fork();

        if(pid==0)
        {
            printf("i'm in the second child, which will be used to run grep\n");
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[READ_END]);
            execlp("grep", "grep", "rc",NULL);
        }
    }

    return 0;
}
