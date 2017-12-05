

#ifndef __CSTRING_H__INCLUDE__
#define __CSTRING_H__INCLUDE__


#include <stdarg.h>
#include <stdint.h>
#include <string.h>


typedef char* cstring_t;


#ifdef __GNUC__
struct __attribute__((__packed__)) cstring_header_5_s {
	unsigned char flags;
	char buf[];
};

struct __attribute__((__packed__)) cstring_header_8_s {
	uint8_t len;
	uint8_t alloc;
	unsigned char flags;
	char buf[];
};

struct __attribute__((__packed__)) cstring_header_16_s {
	uint16_t len;
	uint16_t alloc;
	unsigned char flags;
	char buf[];
};

struct __attribute__((__packed__)) cstring_header_32_s {
	uint32_t len;
	uint32_t alloc;
	unsigned char flags;
	char buf[];
};

struct __attribute__((__packed__)) cstring_header_64_s {
	uint64_t len;
	uint64_t alloc;
	unsigned char flags;
	char buf[];
};
#else
#pragma pack(push)
#pragma pack(1)

struct cstring_header_5_s {
	unsigned char flags;
	char buf[];
};

struct cstring_header_8_s {
	uint8_t len;
	uint8_t alloc;
	unsigned char flags;
	char buf[];
};

struct cstring_header_16_s {
	uint16_t len;
	uint16_t alloc;
	unsigned char flags;
	char buf[];
};

struct cstring_header_32_s {
	uint32_t len;
	uint32_t alloc;
	unsigned char flags;
	char buf[];
};

struct cstring_header_64_s {
	uint64_t len;
	uint64_t alloc;
	unsigned char flags;
	char buf[];
};

#pragma pack(pop)
#endif


#define CSTRING_TYPE_5		0
#define CSTRING_TYPE_8		1
#define CSTRING_TYPE_16		2
#define CSTRING_TYPE_32		3
#define CSTRING_TYPE_64		4
#define CSTRING_TYPE_MASK	7
#define CSTRING_TYPE_BITS	3
#define cstring_header_def(T, s)    struct cstring_header_##T##_s *sh = (void*)((s)-(sizeof(struct cstring_header_##T##_s)));
#define cstring_of(T, s)	        ((struct cstring_header_##T##_s *)((s)-(sizeof(struct cstring_header_##T##_s))))
#define CSTRING_TYPE_5_LENGTH(f)    ((f) >> CSTRING_TYPE_BITS)


static inline 
size_t cstring_length(const cstring_t cs)
{
	unsigned char flags = cs[-1];
	switch (flags & CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5:
		return CSTRING_TYPE_5_LENGTH(flags);
	case CSTRING_TYPE_8:
		return cstring_of(8, cs)->len;
	case CSTRING_TYPE_16:
		return cstring_of(16, cs)->len;
	case CSTRING_TYPE_32:
		return cstring_of(32, cs)->len;
	case CSTRING_TYPE_64:
		return cstring_of(64, cs)->len;
	}
	return 0;
}


static inline 
size_t cstring_avail(const cstring_t cs)
{
	unsigned char flags = cs[-1];
	switch (flags & CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5: {
		return 0;
	}
	case CSTRING_TYPE_8: {
		cstring_header_def(8, cs);
		return sh->alloc - sh->len;
	}
	case CSTRING_TYPE_16: {
		cstring_header_def(16, cs);
		return sh->alloc - sh->len;
	}
	case CSTRING_TYPE_32: {
		cstring_header_def(32, cs);
		return sh->alloc - sh->len;
	}
	case CSTRING_TYPE_64: {
		cstring_header_def(64, cs);
		return sh->alloc - sh->len;
	}
	}
	return 0;
}


static inline 
void cstring_set_length(cstring_t cs, size_t n)
{
	unsigned char flags = cs[-1];
	switch (flags & CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5: {
		unsigned char *fp = ((unsigned char*)cs) - 1;
		*fp = CSTRING_TYPE_5 | (n << CSTRING_TYPE_BITS);
		break;
	}
	case CSTRING_TYPE_8: {
		cstring_of(8, cs)->len = n;
		break;
	}
	case CSTRING_TYPE_16: {
		cstring_of(16, cs)->len = n;
		break;
	}
	case CSTRING_TYPE_32: {
		cstring_of(32, cs)->len = n;
		break;
	}
	case CSTRING_TYPE_64: {
		cstring_of(64, cs)->len = n;
		break;
	}
	}
}


static inline
void cstring_increase_length(cstring_t cs, size_t inc)
{
	unsigned char flags = cs[-1];
	switch (flags & CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5: {
		unsigned char *fp = ((unsigned char*)cs) - 1;
		unsigned char newlen = CSTRING_TYPE_5_LENGTH(flags) + inc;
		*fp = CSTRING_TYPE_5 | (newlen << CSTRING_TYPE_BITS);
	    break;
    }
	case CSTRING_TYPE_8:
		cstring_of(8, cs)->len += inc;
		break;
	case CSTRING_TYPE_16:
		cstring_of(16, cs)->len += inc;
		break;
	case CSTRING_TYPE_32:
		cstring_of(32, cs)->len += inc;
		break;
	case CSTRING_TYPE_64:
		cstring_of(64, cs)->len += inc;
		break;
	}
}


static inline
size_t cstring_alloc(const cstring_t s)
{
	unsigned char flags = s[-1];
	switch (flags&CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5:
		return CSTRING_TYPE_5_LENGTH(flags);
	case CSTRING_TYPE_8:
		return cstring_of(8, s)->alloc;
	case CSTRING_TYPE_16:
		return cstring_of(16, s)->alloc;
	case CSTRING_TYPE_32:
		return cstring_of(32, s)->alloc;
	case CSTRING_TYPE_64:
		return cstring_of(64, s)->alloc;
	}
	return 0;
}


static inline
void cstring_set_alloc(cstring_t s, size_t newlen)
{
	unsigned char flags = s[-1];
	switch (flags & CSTRING_TYPE_MASK) {
	case CSTRING_TYPE_5:
		break;
	case CSTRING_TYPE_8:
		cstring_of(8, s)->alloc = newlen;
		break;
	case CSTRING_TYPE_16:
		cstring_of(16, s)->alloc = newlen;
		break;
	case CSTRING_TYPE_32:
		cstring_of(32, s)->alloc = newlen;
		break;
	case CSTRING_TYPE_64:
		cstring_of(64, s)->alloc = newlen;
		break;
	}
}


cstring_t cstring_new_n(const void *data, size_t n);
cstring_t cstring_new(const char *data);
cstring_t cstring_empty(void);
cstring_t cstring_dup(const cstring_t cs);
void cstring_free(cstring_t cs);
cstring_t cstring_growzero(cstring_t cs, size_t n);
cstring_t cstring_cat_n(cstring_t cs, const void *t, size_t len);
cstring_t cstring_cat(cstring_t cs, const char *t);
cstring_t cstring_cat_cs(cstring_t cs, const cstring_t t);
cstring_t cstring_cat_char(cstring_t cs, char ch);
cstring_t cstring_copy_n(cstring_t cs, const char *t, size_t len);
cstring_t cstring_copy(cstring_t cs, const char *t);

cstring_t cstring_cat_vprintf(cstring_t cs, const char *fmt, va_list ap);
#ifdef __GNUC__
cstring_t cstring_cat_printf(cstring_t cs, const char *fmt, ...)
__attribute__((format(printf, 2, 3)));
#else
cstring_t cstring_cat_printf(cstring_t cs, const char *fmt, ...);
#endif

cstring_t cstring_catfmt(cstring_t cs, char const *fmt, ...);
cstring_t cstring_trim(cstring_t cs, const char *cset);
cstring_t cstring_exclude(cstring_t cs, const char *cset);
void cstring_range(cstring_t cs, int start, int end);
void cstring_update_length(cstring_t cs);
void cstring_clear(cstring_t cs);
int cstring_compare(const cstring_t s1, const cstring_t s2);
cstring_t *cstring_splinters(const char *s, int len, const char *sep, int seplen, int *count);
void cstring_free_splinters(cstring_t *tokens, int count);
void cstring_tolower(cstring_t cs);
void cstring_toupper(cstring_t cs);
cstring_t cstring_from_longlong(long long value);
cstring_t cstring_from_ulonglong(unsigned long long value, int base);
cstring_t cstring_cat_repr(cstring_t cs, const char *p, size_t len);
cstring_t *cstring_split_args(const char *line, int *argc);
cstring_t cstring_map_chars(cstring_t cs, const char *from, const char *to, size_t setlen);
cstring_t cstring_join(char **argv, int argc, char *sep);
cstring_t cstring_join_cs(cstring_t *argv, int argc, const char *sep, size_t seplen);


cstring_t cstring_make_room_for(cstring_t cs, size_t addlen);
void cstring_increase_string_length(cstring_t cs, int incr);
cstring_t cstring_shrink_to_fit(cstring_t cs);
size_t cstring_alloc_size(cstring_t cs);
void *cstring_alloc_ptr(cstring_t cs);


int lltostr(char *s, long long value);
int ulltostr(char *s, unsigned long long v);
size_t xulltostr(char *s, unsigned long long v, int base);


#endif
