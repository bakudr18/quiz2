#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "bitcpy.h"

static uint8_t output[8], input[8], output_align[8];

static inline void dump_8bits(uint8_t _data)
{
    for (int i = 0; i < 8; ++i)
        printf("%d", (_data & (0x80 >> i)) ? 1 : 0);
    printf(" ");
}

static inline void dump_binary(uint8_t *_buffer, size_t _length)
{
    for (int i = 0; i < _length; ++i)
        dump_8bits(*_buffer++);
}

static void validate(uint8_t *r1, uint8_t *r2, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        assert(*r1++ == *r2++);
    }
}

int main(int _argc, char **_argv)
{
    memset(&input[0], 0xFF, sizeof(input));

    for (int i = 1; i <= 32; ++i) {
        for (int j = 0; j < 16; ++j) {
            for (int k = 0; k < 16; ++k) {
                printf("%2d:%2d:%2d ", i, k, j);
                memset(&output[0], 0x00, sizeof(output));
                bitcpy(&output[0], k, &input[0], j, i);
                dump_binary(&output[0], 8);
                printf("\n");

                printf("%2d:%2d:%2d ", i, k, j);
                memset(&output_align[0], 0x00, sizeof(output));
                bitcpy_align(&output_align[0], k, &input[0], j, i);
                dump_binary(&output_align[0], 8);
                printf("\n");

                validate(output, output_align, 8);
            }
        }
    }

    return 0;
}
