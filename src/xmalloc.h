

#ifndef __PALLOC__H__INCLUDE__
#define __PALLOC__H__INCLUDE__


#include "config.h"


typedef void(*xalloc_oom_handler_pt)(const char *fn, long line, void *ud);


#if defined(USE_MALLOC)

#   define xmalloc(size)                        malloc(size)
#   define xcalloc(nmemb, size)                 calloc((nmemb), (size))
#   define xrealloc(ptr, size)                  realloc(ptr, size)
#   define xfree(ptr)                           free(ptr)
#   define set_xalloc_oom_handler(handler, ud)

#else

#   define xmalloc(size)                    x_malloc(__FILE__, __LINE__, (size))
#   define xcalloc(nmemb, size)             x_calloc(__FILE__, __LINE__, (nmemb), (size))
#   define xrealloc(ptr, size)              x_realloc(__FILE__, __LINE__, (ptr), (size))
#   define xfree(ptr)                       x_free(__FILE__, __LINE__, (ptr))

void* x_malloc(const char *fn, long line, size_t size);
void* x_calloc(const char *fn, long line, size_t nmemb, size_t size);
void* x_realloc(const char *fn, long line, void *ptr, size_t size);
void x_free(const char *fn, long line, void *ptr);
void set_xalloc_oom_handler(xalloc_oom_handler_pt handler, void *ud);

#endif


#endif
