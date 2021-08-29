#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

/* #define REPEAT 1 */
/* #define REPEAT 10 */
#define REPEAT 100000

double tv_to_microseconds(struct timeval tv) {
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void repeat(void (*f)(), ...) {
    va_list args;
    va_list args_copy;
    va_start(args, f);

    int i;
    for (i = 0; i < REPEAT; ++i) {
        va_copy(args_copy, args);
        (*f)(args_copy);
        va_end(args_copy);
    }

    va_end(args);
}

struct Intervals {
    struct timeval start_time;
    struct timeval end_time;
} interval;

void start_mesure() {
    gettimeofday(&interval.start_time, NULL);
}

double end_mesure() {
    gettimeofday(&interval.end_time, NULL);
    return (tv_to_microseconds(interval.end_time) - tv_to_microseconds(interval.start_time)) / REPEAT;
}

double measure(void (*f)(), ...) {
    start_mesure();

    va_list args;
    va_start(args, f);
    (*f)(args);
    va_end(args);

    return end_mesure();
};
