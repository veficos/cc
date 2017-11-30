

#ifndef __ARRAY__H__INCLUDE__
#define __ARRAY__H__INCLUDE__


#include "config.h"


typedef struct array_s {
    void   *elts;
    size_t  nelts;
    size_t  size;
    size_t  nalloc;
} array_t;


#define array_pop_back(a)                           \
    do {                                            \
        if ((a)->nelts > 0) {                       \
            (a)->nelts--;                           \
        }                                           \
    } while(0)


#define array_pop_back_n(a, n)                      \
    do {                                            \
        if ((n) > (a)->nelts) {                     \
            (a)->nelts = 0;                         \
        } else {                                    \
            (a)->nelts -= n;                        \
        }                                           \
    } while(0)


#define array_entry(a, type)                        \
    (a ? ((type*)((a)->elts)) : NULL)


#define array_length(a)                             \
    ((a)->nelts)


#define array_capacity(a)                           \
    ((a)->nalloc - (a)->nelts)


#define array_clear(a)                              \
    ((a)->nelts = 0)


#define array_is_empty(a)                           \
    ((a)->nelts == 0)


#define array_foreach(a, base, index)               \
    for ((base) = (a)->elts, (index) = 0;           \
         (size_t)(index) < (a)->nelts;              \
         (index)++)


#define array_at(a, index)                          \
    __array_at(a, index)


#define array_front(a)                              \
    __array_front(a)


#define array_back(a)                               \
    __array_back(a)


#define array_cast_at(type, a, index)               \
    (*((type*)__array_at(a, index)))


#define array_cast_front(type, a)                   \
    (*((type*)__array_front(a)))
    

#define array_cast_back(type, a)                    \
    (*((type*)__array_back(a)))


#define array_cast_append(type, a, element)         \
    do {                                            \
        (*(type*)array_push_back((a))) = element;   \
    } while (false)


array_t* array_create(size_t size);
array_t* array_create_n(size_t size, size_t n);
void array_destroy(array_t *a);
void *array_push_back(array_t *a);
void *array_push_back_n(array_t *a, size_t n);
bool array_extend(array_t *a, array_t *b);


static inline
void* __array_at(array_t *a, size_t i)
{
    assert(a->nelts > i);
    return (((unsigned char *)a->elts) + (a->size * i));
}


static inline
void* __array_front(array_t *a)
{
    assert(a->nelts > 0);
    return a->elts;
}


static inline
void* __array_back(array_t *a)
{
    assert(a->nelts > 0);
    return (((unsigned char *)a->elts) + (a->size * (a->nelts - 1)));
}


#endif
