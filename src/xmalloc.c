

#include "config.h"

#include "xmalloc.h"
#include "color.h"


#ifndef USE_MALLOC


static inline
void __default_alloc_oom_handler__(const char *fn, long line, void *ud)
{
    fprintf(stderr, "%s:%d: " BRUSH_RED("error: ") "out of memory.\n", fn, line);
    exit(-1);
}


enum {
    idx_alloc_oom_handler = 0,
    idx_user_data,
};


void* xalloc_oom_handler[2] = {
    (void *) __default_alloc_oom_handler__,
    NULL
};


static void __oom_handler__(const char *fn, long line)
{
    xalloc_oom_handler_pt alloc_oom_handler = (xalloc_oom_handler_pt) xalloc_oom_handler[idx_alloc_oom_handler];
    void *ud = xalloc_oom_handler[idx_user_data];

    assert(alloc_oom_handler != NULL);
    alloc_oom_handler(fn, line, xalloc_oom_handler[1]);
}


void* x_malloc(const char *fn, long line, size_t size)
{
    void *ptr;
    
    if ((ptr = malloc(size)) == NULL) {
        __oom_handler__(fn, line);
        return NULL;
    }

    return ptr;
}


void* x_calloc(const char *fn, long line, size_t nmemb, size_t size)
{
    void *ptr;

    if ((ptr = calloc(nmemb, size)) == NULL) {
        __oom_handler__(fn, line);
        return NULL;
    }

    return ptr;
}


void* x_realloc(const char *fn, long line, void *ptr, size_t size)
{
    if ((ptr = realloc(ptr, size)) == NULL) {
        __oom_handler__(fn, line);
        return NULL;
    }
    return ptr;
}


void x_free(const char *fn, long line, void *ptr)
{
    free(ptr);
}


void set_xalloc_oom_handler(xalloc_oom_handler_pt handler, void *ud)
{
    xalloc_oom_handler[idx_alloc_oom_handler] = (void*) handler;
    xalloc_oom_handler[idx_user_data] = ud;
}


#endif
