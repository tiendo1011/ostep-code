#define _GNU_SOURCE
#include <sched.h>

void use_cpu_core(int core) {
    // Taken from: https://stackoverflow.com/a/11271253/4632735
    // Use CPU 1 only
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    CPU_SET(core, &my_set);
    sched_setaffinity(getpid(), sizeof(cpu_set_t), &my_set);
}
