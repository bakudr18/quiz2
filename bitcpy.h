#ifndef _BITCPY_H
#define _BITCPY_H

#include <string.h>

void bitcpy(void *_dest,
            size_t _write,
            const void *_src,
            size_t _read,
            size_t count);

void bitcpy_branch_predict(void *_dest,
                           size_t _write,
                           const void *_src,
                           size_t _read,
                           size_t count);

void bitcpy_align(void *_dest,
                  size_t _write,
                  const void *_src,
                  size_t _read,
                  size_t count);

#endif
