#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int) getpid());
    FILE *fp;
    fp = fopen("./p4.output", "w");
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        fprintf(fp, "from child\n");
    } else {
        int wc = wait(NULL);
        // parent goes down this path (original process)
        printf("hello, I am parent of %d (pid:%d)\n",
	       rc, (int) getpid());
        fprintf(fp, "from parent\n");
    }
    return 0;
}

// Can both the child and parent access the file descriptor returned by open()?
// Yes
//
// What happens when they are writing to the file concurrently, i.e., at the same time?
// they don't override each other, even if the file is openned with "w" mode
// => The file is override only when another process re-open the file
// so when you open a file, you decide if you wanna override it, or append to it
//
// so file descriptor is sharable
// this makes sense since lot's of processes sharing the same
// stdin, stdout, stderr
