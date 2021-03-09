#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void bitcpy(void *_dest,      /* Address of the buffer to write to */
            size_t _write,    /* Bit offset to start writing to */
            const void *_src, /* Address of the buffer to read from */
            size_t _read,     /* Bit offset to start reading from */
            size_t count)
{
    size_t read_lhs = _read & 7;
    size_t read_rhs = 8 - read_lhs;
    const uint8_t *source = (const uint8_t *) _src + (_read / 8);
    size_t write_lhs = _write & 7;
    size_t write_rhs = 8 - write_lhs;
    uint8_t *dest = (uint8_t *) _dest + (_write / 8);

    static const uint8_t read_mask[] = {
        0x00, /*    == 0    00000000b   */
        0x80, /*    == 1    10000000b   */
        0xC0, /*    == 2    11000000b   */
        0xE0, /*    == 3    11100000b   */
        0xF0, /*    == 4    11110000b   */
        0xF8, /*    == 5    11111000b   */
        0xFC, /*    == 6    11111100b   */
        0xFE, /*    == 7    11111110b   */
        0xFF  /*    == 8    11111111b   */
    };

    static const uint8_t write_mask[] = {
        0xFF, /*    == 0    11111111b   */
        0x7F, /*    == 1    01111111b   */
        0x3F, /*    == 2    00111111b   */
        0x1F, /*    == 3    00011111b   */
        0x0F, /*    == 4    00001111b   */
        0x07, /*    == 5    00000111b   */
        0x03, /*    == 6    00000011b   */
        0x01, /*    == 7    00000001b   */
        0x00  /*    == 8    00000000b   */
    };

    while (count > 0) {
        uint8_t data = *source++;
        size_t bitsize = (count > 8) ? 8 : count;
        if (read_lhs > 0) {
            data <<= read_lhs;
            if (bitsize > read_rhs)
                data |= (*source >> read_rhs);
        }

        if (bitsize < 8)
            data &= read_mask[bitsize];

        uint8_t original = *dest;
        uint8_t mask = read_mask[write_lhs];
        if (bitsize > write_rhs) {
            /* Cross multiple bytes */
            *dest++ = (original & mask) | (data >> write_lhs);
            original = *dest & write_mask[bitsize - write_rhs];
            *dest = original | (data << write_rhs);
        } else {
            // Since write_lhs + bitsize is never >= 8, no out-of-bound access.
            mask |= write_mask[write_lhs + bitsize];
            *dest++ = (original & mask) | (data >> write_lhs);
        }

        count -= bitsize;
    }
}

void bitcpy_branch_predict(
    void *_dest,      /* Address of the buffer to write to */
    size_t _write,    /* Bit offset to start writing to */
    const void *_src, /* Address of the buffer to read from */
    size_t _read,     /* Bit offset to start reading from */
    size_t count)
{
    size_t read_lhs = _read & 7;
    size_t read_rhs = 8 - read_lhs;
    const uint8_t *source = (const uint8_t *) _src + (_read / 8);
    size_t write_lhs = _write & 7;
    size_t write_rhs = 8 - write_lhs;
    uint8_t *dest = (uint8_t *) _dest + (_write / 8);

#define READMASK(x) (uint8_t)((~0U) << (8 - (x)))
#define WRITEMASK(x) (uint8_t)((~0U) >> (x))

    while (count > 0) {
        uint8_t data = *source++;
        size_t bitsize = (count > 8) ? 8 : count;
        data = (data << read_lhs) | (*source >> read_rhs);
        data &= READMASK(bitsize);

        uint8_t mask = READMASK(write_lhs);
        uint8_t original = *dest;
        *dest++ = (original & mask) | (data >> write_lhs);
        *dest &= WRITEMASK(bitsize - write_rhs);
        *dest |= (data << write_rhs);

        count -= bitsize;
    }
}

void bitcpy_align(void *_dest,      /* Address of the buffer to write to */
                  size_t _write,    /* Bit offset to start writing to */
                  const void *_src, /* Address of the buffer to read from */
                  size_t _read,     /* Bit offset to start reading from */
                  size_t count)
{
    size_t read_lhs = _read & 7;
    size_t read_rhs = 8 - read_lhs;
    const uint8_t *source = (const uint8_t *) _src + (_read / 8);
    size_t write_lhs = _write & 7;
    size_t write_rhs = 8 - write_lhs;
    uint8_t *dest = (uint8_t *) _dest + (_write / 8);

#define READMASK(x) (uint8_t)((~0U) << (8 - (x)))
#define WRITEMASK(x) (uint8_t)((~0U) >> (x))

    if (read_lhs == write_lhs) {
        uint8_t mask;
        uint8_t data = *source++;
        uint8_t original = *dest;
        size_t bitsize = count > read_rhs ? read_rhs : count;
        data = (data << read_lhs) & READMASK(bitsize);
        mask = WRITEMASK(bitsize);
        *dest++ = (original & mask) | (data >> read_lhs);
        count -= bitsize;

        size_t bytecount = count / 8;
        if (bytecount > 0) {
            memcpy(dest, source, bytecount);
            source += bytecount;
            dest += bytecount;
        }

        data = *source;
        original = *dest;
        count %= 8;
        if (count > 0) {
            mask = READMASK(count);
            *dest = (original & (~mask)) | (data & mask);
        }
    } else {
        while (count > 0) {
            uint8_t data = *source++;
            size_t bitsize = (count > 8) ? 8 : count;
            data = (data << read_lhs) | (*source >> read_rhs);
            data &= READMASK(bitsize);

            uint8_t mask = READMASK(write_lhs);
            uint8_t original = *dest;
            *dest++ = (original & mask) | (data >> write_lhs);
            *dest &= WRITEMASK(bitsize - write_rhs);
            *dest |= (data << write_rhs);

            count -= bitsize;
        }
    }
}

static uint8_t output[8], input[8];

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

int main(int _argc, char **_argv)
{
    memset(&input[0], 0xFF, sizeof(input));

    for (int i = 1; i <= 32; ++i) {
        for (int j = 0; j < 16; ++j) {
            for (int k = 0; k < 16; ++k) {
                memset(&output[0], 0x00, sizeof(output));
                printf("%2d:%2d:%2d ", i, k, j);
                bitcpy_align(&output[0], k, &input[0], j, i);
                dump_binary(&output[0], 8);
                printf("\n");
            }
        }
    }

    return 0;
}

