

#ifndef __CSTRINGPOOL__H__INCLUDE__
#define __CSTRINGPOOL__H__INCLUDE__


#include "config.h"
#include "avltree.h"
#include "cstring.h"


typedef struct cspool_s {
    avltree_t tree;
} cspool_t;


static inline
void cspool_init(cspool_t *cp)
{
    avltree_init(&cp->tree);
}


void cspool_uninit(cspool_t *cp);
cstring_t cspool_get(cspool_t *cp, const void *data, size_t n);


#endif
