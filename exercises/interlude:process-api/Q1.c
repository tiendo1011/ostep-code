#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int x = 0;
int
main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int) getpid());
    x = 100;
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        printf("x is %d\n", x);
        x = 1;
        printf("after change to 1, x is %d\n", x);
    } else {
        int wc = wait(NULL);
        // parent goes down this path (original process)
        printf("hello, I am parent of %d (pid:%d)\n",
	       rc, (int) getpid());
        printf("x is %d\n", x);
    }
    return 0;
}

// What value is the variable in the child progress?
// is the same as the parent
//
// What happens to the variable when both the child and parent change the value of x?
// it won't affect each other
// => after the fork, each process has its own address space
