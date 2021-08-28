#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

    int
main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int) getpid());
    int fd[2];
    pipe(fd);

    int rc = fork();

    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // first child will have these streams when start:
        // stdin: 0
        // stdout: 1
        // stderr: 2
        // fd[READ_END]: 3
        // fd[WRITE_END]: 4

        dup2(fd[WRITE_END], STDOUT_FILENO);
        // after dup2, first child will have these streams:
        // stdin: 0
        // copied of fd[WRITE_END]: 1
        // stderr: 2
        // fd[READ_END]: 3
        // fd[WRITE_END]: 4

        close(fd[READ_END]);
        close(fd[WRITE_END]);
        // after close, first child will have these streams:
        // stdin: 0
        // copied of fd[WRITE_END]: 1
        // stderr: 2

        // By default, output will be send to file descriptor 1
        // which is the `copied of fd[WRITE_END]`
        // which points to the write end of the pipe
        printf("1Hello, I am the first child (pid:%d)\n", (int) getpid());
    } else {
        int rc1 = fork();
        if (rc1 < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc1 == 0) {
            // second child will have these streams when start:
            // stdin: 0
            // stdout: 1
            // stderr: 2
            // fd[READ_END]: 3
            // fd[WRITE_END]: 4

            dup2(fd[READ_END], STDIN_FILENO);
            // after dup2, first child will have these streams:
            // copied of fd[READ_END]: 0
            // sdtout: 1
            // stderr: 2
            // fd[READ_END]: 3
            // fd[WRITE_END]: 4

            close(fd[READ_END]);
            close(fd[WRITE_END]);
            // after close, first child will have these streams:
            // copied of fd[READ_END]: 0
            // sdtout: 1
            // stderr: 2

            // By default, input will be taken from file descriptor 0
            // which is the `copied of fd[READ_END]`
            // which points to the read end of the pipe

            // This gets character from stdin, which currently is fd[READ_END]
            char c = getc(stdin);

            // And print it to stdout
            putc(c, stdout);

            printf("hello, I am the second child (pid:%d)\n", (int) getpid());
        } else {
            int wc = wait(NULL);
            // parent goes down this path (original process)
            printf("hello, I am parent of %d and %d (pid:%d)\n",
                    rc, rc1, (int) getpid());
        }
    }
    return 0;
}

// One interesting point, closing the STDOUT_FILENO in a child process
// won't affect other child process
// it can still write to STDOUT_FILENO
// so each process has its own copy of the file descriptor
// which points to the real device somewhere
//
// when we call close(STDOUT_FILENO)
// it closes the portal to the file
