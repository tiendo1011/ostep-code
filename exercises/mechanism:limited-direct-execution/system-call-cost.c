#include <stdio.h>
#include "./measure.c"

void open_file() {
    fopen("./exercises/mechanism:limited-direct-execution/0-byte", "r");
}

void system_call() {
    repeat(open_file);
}

int
main(int argc, char *argv[])
{
    printf("system call cost %f", measure(system_call));
    return 0;
}

// system call cost ~0.5 microseconds
