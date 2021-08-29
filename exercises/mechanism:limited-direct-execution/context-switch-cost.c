#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include "./measure.c"
#include "./set_cpu_core.c"

#define READ_END 0
#define WRITE_END 1

void consume_pipe(va_list args) {
    int read_fd = va_arg(args, int);
    int write_fd = va_arg(args, int);
    char *order = va_arg(args, char*);

    char str2[strlen(order) + 1];

    if (strcmp(order, "write_then_read") == 0) {
        write(write_fd, order, strlen(order) + 1);
        read(read_fd, str2, strlen(order) + 1);
    } else if (strcmp(order, "read_then_write") == 0) {
        read(read_fd, str2, strlen(order) + 1);
        write(write_fd, order, strlen(order) + 1);
    } else {
        printf("invalid order: %s", order);
    }
}

void piping(va_list args) {
    char *piping_type = va_arg(args, char*);
    int fd[2];
    int fd2[2];
    pipe(fd);
    pipe(fd2);

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
        // fd2[READ_END]: 5
        // fd2[WRITE_END]: 6
        use_cpu_core(1);
        printf("first child cpu core: %d\n", sched_getcpu());

        printf("first child, %s\n", piping_type);
        if (strcmp(piping_type, "no_context_switch") == 0) {
            repeat(consume_pipe, fd[READ_END], fd[WRITE_END], "write_then_read");
        } else if (strcmp(piping_type, "with_context_switch") == 0) {
            repeat(consume_pipe, fd2[READ_END], fd[WRITE_END], "write_then_read");
        } else {
            printf("invalid piping_type: %s\n", piping_type);
        }
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
            // fd2[READ_END]: 5
            // fd2[WRITE_END]: 6

            use_cpu_core(1);
            printf("second child cpu core: %d\n", sched_getcpu());

            printf("second child, %s\n", piping_type);
            if (strcmp(piping_type, "no_context_switch") == 0) {
                repeat(consume_pipe, fd2[READ_END], fd2[WRITE_END], "write_then_read");
            } else if (strcmp(piping_type, "with_context_switch") == 0) {
                repeat(consume_pipe, fd[READ_END], fd2[WRITE_END], "read_then_write");
            } else {
                printf("invalid piping_type: %s\n", piping_type);
            }
        } else {
            int wc = wait(NULL);
            printf("from parent\n");
        }
    }
}

int main(int argc, char *argv[]) {
    /* printf("no-context-switch total cost: %f\n", measure(piping, "no_context_switch")); */
    printf("with-context-switch total cost: %f\n", measure(piping, "with_context_switch"));
    return 0;
}
// Note: This will print 3 `total cost` to the console (instead of one),
// the reason is that after piping is called, it forks 2 children
// each children runs its own version of what's after the calls
// including everything after piping is called inside measure
// which includes the printf statement of total cost
// I don't know how to fix it yet
// But I can still use the final result for my calculation

// Result:
// no-context-switch cost: 2.09 * 10^ 5 microseconds
// with-context-switch cost: 5.34 * 10 ^ 5 microseconds
// if each child's number of calls to pipe is x
// then the context-switch is 2x
// then context switch cost = (time_taken_with_context_switch - time_taken_without_context_switch) / number_of_context_switch
// (5.34 * 10 ^ 5 - 2.09 * 10 ^ 5) / (10 ^ 5 * 2)
// = (5.34 - 2.09) / 2
// = 1.625 (microseconds)
