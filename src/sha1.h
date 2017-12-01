

#ifndef __SHA1__H__INCLUDE__
#define __SHA1__H__INCLUDE__


/*
#define SHA1_INDEPENDENT_MODULE
*/


#ifndef SHA1_INDEPENDENT_MODULE
#include "config.h"
#else
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#endif


typedef struct sha1_s {
    uint64_t  bytes;
    uint32_t  a, b, c, d, e, f;
    uint8_t   buffer[64];
} sha1_t;


void sha1_init(sha1_t *ctx);
void sha1_update(sha1_t *ctx, const void *data, size_t size);
void sha1_final(sha1_t *ctx, uint8_t result[20]);


static
void sha1_fast(const void *data, size_t size, uint8_t result[20])
{
    sha1_t sha1;

    sha1_init(&sha1);

    sha1_update(&sha1, data, size);

    sha1_final(&sha1, result);
}


#endif
