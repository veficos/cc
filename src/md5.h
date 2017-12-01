

#ifndef __MD5__H__INCLUDE__
#define __MD5__H__INCLUDE__


/*
#define MD5_INDEPENDENT_MODULE
*/


#ifndef MD5_INDEPENDENT_MODULE
#include "config.h"
#else
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#endif


typedef struct md5_s {
    uint64_t  bytes;
    uint32_t  a, b, c, d;
    uint8_t   buffer[64];
} md5_t;


void md5_init(md5_t *ctx);
void md5_update(md5_t *ctx, const void *data, size_t size);
void md5_final(md5_t *ctx, uint8_t result[16]);


static
void md5_fast(const void *data, size_t size, uint8_t result[16])
{
    md5_t md5;

    md5_init(&md5);

    md5_update(&md5, data, size);

    md5_final(&md5, result);
}


#endif
