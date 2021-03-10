#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitcpy.h"

static uint8_t input[4096], output[4096];

static inline long long elapsed(struct timespec t1, struct timespec t2)
{
    return (long long) (t2.tv_sec - t1.tv_sec) * 1e9 +
           (long long) (t2.tv_nsec - t1.tv_nsec);
}

int main(int argc, char **argv)
{
    struct timespec t1, t2;
    memset(&input[0], 0xFF, sizeof(input));

    for (int i = 1; i < 8000; i++) {
        size_t r, w;
        r = rand() % 8;
        w = rand() % 8;
        memset(&output[0], 0x00, sizeof(output));
        clock_gettime(CLOCK_REALTIME, &t1);
        bitcpy(&output[0], w, &input[0], r, i);
        clock_gettime(CLOCK_REALTIME, &t2);
        printf("%d %lld ", i, elapsed(t1, t2));

        memset(&output[0], 0x00, sizeof(output));
        clock_gettime(CLOCK_REALTIME, &t1);
        bitcpy_align(&output[0], w, &input[0], r, i);
        clock_gettime(CLOCK_REALTIME, &t2);
        printf("%lld ", elapsed(t1, t2));

        memset(&output[0], 0x00, sizeof(output));
        clock_gettime(CLOCK_REALTIME, &t1);
        bitcpy_branch_predict(&output[0], w, &input[0], r, i);
        clock_gettime(CLOCK_REALTIME, &t2);
        printf("%lld\n", elapsed(t1, t2));
    }

    return 0;
}
