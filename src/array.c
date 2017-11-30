

#include "config.h"
#include "array.h"
#include "xmalloc.h"


static inline int __array_resize__(array_t *a, size_t n);


array_t* array_create(size_t size)
{
    array_t *a;

    if ((a = (array_t*) xmalloc(sizeof(array_t))) == NULL) {
        return NULL;
    }

    a->elts   = NULL;
    a->nelts  = 0;
    a->nalloc = 0;
    a->size   = size;
    return a;
}


array_t* array_create_n(size_t size, size_t n)
{
    array_t *a;

    if ((a = (array_t*) xmalloc(sizeof(array_t))) == NULL) {
        return NULL;
    }

    if ((a->elts = xmalloc(size * n)) == NULL) {
        xfree(a);
        return NULL;
    }

    a->nelts   = 0;
    a->nalloc  = n;
    a->size    = size;
    return a;
}


void array_destroy(array_t *a)
{
    assert(a && a->elts);

    if (a->elts != NULL) {
        xfree(a->elts);
    }

    xfree(a);
}


void* array_push_back(array_t *a)
{
    void *elt;

    if (a->nelts == a->nalloc) {
        size_t n;

        if (a->nalloc == 0) {
            n = 2;

        } else {
            n = a->nalloc * 2;
        }

        if (!__array_resize__(a, n)) {
            return NULL;
        }
    }

    elt = (char *) a->elts + a->size * a->nelts;
    a->nelts++;
    return elt;
}


void* array_push_back_n(array_t *a, size_t n)
{
    void *elt;

    if (a->nelts + n > a->nalloc) {
        size_t nalloc = 2 * (n >= a->nalloc ? n : a->nalloc);

        if (!__array_resize__(a, nalloc)) {
            return NULL;
        }
    }

    elt = (unsigned char *)a->elts + a->size * a->nelts;
    a->nelts += n;
    return elt;
}


bool array_extend(array_t *a, array_t *b)
{
    void *chunk;

    if (a->size != b->size) {
        return false;
    }

    if ((chunk = array_push_back_n(a, b->nelts)) == NULL) {
        return false;
    }

    memcpy(chunk, b->elts, b->size * b->nelts);

    return true;
}


static inline
int __array_resize__(array_t *a, size_t n)
{
    size_t resize  = a->size * n;

    if (resize) {
        if ((a->elts = xrealloc(a->elts, resize)) == NULL) {
            return 0;
        }
    }
    
    a->nalloc = n;
    return n;
}
