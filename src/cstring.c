

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include <malloc.h>

#include "cstring.h"


#ifndef CSTRING_MAX_PREALLOC
#define CSTRING_MAX_PREALLOC            (1024 * 1024)
#endif

#ifndef CSTRING_LONGLLONG_STRING_SIZE
#define CSTRING_LONGLLONG_STRING_SIZE   (21)
#endif

#ifndef xmalloc
#define xmalloc     malloc
#endif

#ifndef xfree
#define xfree       free
#endif

#ifndef xrealloc
#define xrealloc    realloc
#endif


static inline
int cstring_header_size(char type)
{
    switch (type & CSTRING_TYPE_MASK) {
    case CSTRING_TYPE_5:
        return sizeof(struct cstring_header_5_s);
    case CSTRING_TYPE_8:
        return sizeof(struct cstring_header_8_s);
    case CSTRING_TYPE_16:
        return sizeof(struct cstring_header_16_s);
    case CSTRING_TYPE_32:
        return sizeof(struct cstring_header_32_s);
    case CSTRING_TYPE_64:
        return sizeof(struct cstring_header_64_s);
    }
    return 0;
}


static inline
char cstring_req_type(size_t string_size)
{
    if (string_size < 1 << 5)
        return CSTRING_TYPE_5;
    if (string_size < 1 << 8)
        return CSTRING_TYPE_8;
    if (string_size < 1 << 16)
        return CSTRING_TYPE_16;
#if (LONG_MAX == LLONG_MAX)
    if (string_size < 1ll << 32)
        return CSTRING_TYPE_32;
#endif
    return CSTRING_TYPE_64;
}


cstring_t cstring_new_n(const void *data, size_t n)
{
    void *sh;
    cstring_t cs;
    char type;
    int hdrlen;
    unsigned char *fp;

    type = cstring_req_type(n);
    if (type == CSTRING_TYPE_5 && n == 0) {
        type = CSTRING_TYPE_8;
    }

    hdrlen = cstring_header_size(type);

    sh = xmalloc(hdrlen + n + 1);
    if (!data) {
        memset(sh, 0, hdrlen + n + 1);
    }

    if (sh == NULL) {
        return NULL;
    }

    cs = (char*)sh + hdrlen;
    fp = ((unsigned char*)cs) - 1;

    switch (type) {
    case CSTRING_TYPE_5: {
        *fp = type | (n << CSTRING_TYPE_BITS);
        break;
    }
    case CSTRING_TYPE_8: {
        cstring_header_def(8, cs);
        sh->len = n;
        sh->alloc = n;
        *fp = type;
        break;
    }
    case CSTRING_TYPE_16: {
        cstring_header_def(16, cs);
        sh->len = n;
        sh->alloc = n;
        *fp = type;
        break;
    }
    case CSTRING_TYPE_32: {
        cstring_header_def(32, cs);
        sh->len = n;
        sh->alloc = n;
        *fp = type;
        break;
    }
    case CSTRING_TYPE_64: {
        cstring_header_def(64, cs);
        sh->len = n;
        sh->alloc = n;
        *fp = type;
        break;
    }
    }

    if (n && data) {
        memcpy(cs, data, n);
    }

    cs[n] = '\0';
    return cs;
}


cstring_t cstring_empty(void)
{
    return cstring_new_n("", 0);
}


cstring_t cstring_new(const char *s)
{
    size_t n = (s == NULL) ? 0 : strlen(s);
    return cstring_new_n(s, n);
}


cstring_t cstring_dup(const cstring_t cs) 
{
    return cstring_new_n(cs, cstring_length(cs));
}


void cstring_free(cstring_t cs) 
{
    if (cs == NULL) {
        return;
    }
    xfree((char*)cs - cstring_header_size(cs[-1]));
}


void cstring_update_length(cstring_t cs)
{
    int reallen = strlen(cs);
    cstring_set_length(cs, reallen);
}


void cstring_clear(cstring_t cs)
{
    cstring_set_length(cs, 0);
    cs[0] = '\0';
}


cstring_t cstring_make_room_for(cstring_t cs, size_t addlen)
{
    void *sh, *newsh;
    size_t avail = cstring_avail(cs);
    size_t len, newlen;
    char type, oldtype;
    int hdrlen;

    if (avail >= addlen) {
        return cs;
    }

    len = cstring_length(cs);
    oldtype = cs[-1] & CSTRING_TYPE_MASK;
    sh = (char*)cs - cstring_header_size(oldtype);
    newlen = (len + addlen);
    if (newlen < CSTRING_MAX_PREALLOC) {
        newlen *= 2;
    } else {
        newlen += CSTRING_MAX_PREALLOC;
    }

    type = cstring_req_type(newlen);
    if (type == CSTRING_TYPE_5) {
        type = CSTRING_TYPE_8;
    }

    hdrlen = cstring_header_size(type);
    if (oldtype == type) {
        newsh = xrealloc(sh, hdrlen + newlen + 1);
        if (newsh == NULL) return NULL;
        cs = (char*)newsh + hdrlen;
    } else {
        newsh = xmalloc(hdrlen + newlen + 1);
        if (newsh == NULL) {
            return NULL;
        }
        memcpy((char*)newsh + hdrlen, cs, len + 1);
        cs = (char*)newsh + hdrlen;
        cs[-1] = type;
        cstring_set_length(cs, len);
        xfree(sh);
    }
    cstring_set_alloc(cs, newlen);
    return cs;
}


cstring_t cstring_shrink_to_fit(cstring_t cs)
{
    void *sh, *newsh;
    char type, oldtype;
    size_t len;

    oldtype = cs[-1] & CSTRING_TYPE_MASK;
    len = cstring_length(cs);
    sh = (char*)cs - cstring_header_size(oldtype);

    type = cstring_req_type(len);
    if (oldtype == type || type > CSTRING_TYPE_8) {
        int oldhdrlen = cstring_header_size(oldtype);
        newsh = xrealloc(sh, oldhdrlen + len + 1);
        if (newsh == NULL) {
            return NULL;
        }
        cs = (char*)newsh + oldhdrlen;

    } else {
        int hdrlen = cstring_header_size(type);
        newsh = xmalloc(hdrlen + len + 1);
        if (newsh == NULL) {
            return NULL;
        }
        memcpy((char*)newsh + hdrlen, cs, len + 1);
        cs = (char*)newsh + hdrlen;
        cs[-1] = type;
        cstring_set_length(cs, len);
        xfree(sh);
    }

    cstring_set_alloc(cs, len);
    return cs;
}


size_t cstring_alloc_size(cstring_t s)
{
    size_t alloc = cstring_alloc(s);
    return cstring_header_size(s[-1]) + alloc + 1;
}


void *cstring_alloc_ptr(cstring_t s)
{
    return (void*)(s - cstring_header_size(s[-1]));
}


void cstring_increase_string_length(cstring_t cs, int incr)
{
    unsigned char flags = cs[-1];
    size_t len;
    switch (flags & CSTRING_TYPE_MASK) {
    case CSTRING_TYPE_5: {
        unsigned char *fp = ((unsigned char*)cs) - 1;
        unsigned char oldlen = CSTRING_TYPE_5_LENGTH(flags);
        assert((incr > 0 && oldlen + incr < 32) || (incr < 0 && oldlen >= (unsigned int)(-incr)));
        *fp = CSTRING_TYPE_5 | ((oldlen + incr) << CSTRING_TYPE_BITS);
        len = oldlen + incr;
        break;
    }
    case CSTRING_TYPE_8: {
        cstring_header_def(8, cs);
        assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
        len = (sh->len += incr);
        break;
    }
    case CSTRING_TYPE_16: {
        cstring_header_def(16, cs);
        assert((incr >= 0 && sh->alloc - sh->len >= incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
        len = (sh->len += incr);
        break;
    }
    case CSTRING_TYPE_32: {
        cstring_header_def(32, cs);
        assert((incr >= 0 && sh->alloc - sh->len >= (unsigned int)incr) || (incr < 0 && sh->len >= (unsigned int)(-incr)));
        len = (sh->len += incr);
        break;
    }
    case CSTRING_TYPE_64: {
        cstring_header_def(64, cs);
        assert((incr >= 0 && sh->alloc - sh->len >= (uint64_t)incr) || (incr < 0 && sh->len >= (uint64_t)(-incr)));
        len = (sh->len += incr);
        break;
    }
    default: len = 0;
    }
    cs[len] = '\0';
}


cstring_t cstring_growzero(cstring_t cs, size_t n)
{
    size_t curlen = cstring_length(cs);

    if (n <= curlen) {
        return cs;
    }

    cs = cstring_make_room_for(cs, n - curlen);
    if (cs == NULL) {
        return NULL;
    }

    memset(cs + curlen, 0, (n - curlen + 1));
    cstring_set_length(cs, n);
    return cs;
}


cstring_t cstring_cat_n(cstring_t cs, const void *data, size_t n)
{
    size_t curlen = cstring_length(cs);

    cs = cstring_make_room_for(cs, n);
    if (cs == NULL) {
        return NULL;
    }

    memcpy(cs + curlen, data, n);
    cstring_set_length(cs, curlen + n);
    cs[curlen + n] = '\0';
    return cs;
}


cstring_t cstring_cat(cstring_t cs, const char *s)
{
    return cstring_cat_n(cs, s, strlen(s));
}


cstring_t cstring_cat_cs(cstring_t dest, const cstring_t src)
{
    return cstring_cat_n(dest, src, cstring_length(src));
}


cstring_t cstring_cat_char(cstring_t cs, char ch)
{
    return cstring_cat_n(cs, &ch, 1);
}


cstring_t cstring_copy_n(cstring_t cs, const char *data, size_t n)
{
    if (cstring_alloc(cs) < n) {
        cs = cstring_make_room_for(cs, n - cstring_length(cs));
        if (cs == NULL) {
            return NULL;
        }
    }

    memcpy(cs, data, n);
    cs[n] = '\0';
    cstring_set_length(cs, n);
    return cs;
}


cstring_t cstring_copy(cstring_t dest, const char *src)
{
    return cstring_copy_n(dest, src, strlen(src));
}


int lltostr(char *s, long long value)
{
    char *p, aux;
    unsigned long long v;
    size_t l;

    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0' + (v % 10);
        v /= 10;
    } while (v);

    if (value < 0) *p++ = '-';

    l = p - s;
    *p = '\0';

    p--;
    while (s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }

    return l;
}


int ulltostr(char *s, unsigned long long v)
{
    char *p, aux;
    size_t l;

    p = s;
    do {
        *p++ = '0' + (v % 10);
        v /= 10;
    } while (v);

    l = p - s;
    *p = '\0';

    p--;
    while (s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}


size_t xulltostr(char *s, unsigned long long v, int base)
{
    char *p, aux, *radix="ABCDEFGHIJKLMNOPQRSTUV";
    size_t l;
    unsigned long long remainder;

    if (base < 0 || base > 32) {
        return 0;
    }

    p = s;
    do {
        remainder = (v % base);
        *p++ = (char) ((base > 10 && remainder >= 10) ? radix[remainder-10] : '0' + remainder);
        v /= base;
    } while (v);

    l = p - s;
    *p = '\0';

    p--;
    while (s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }

    return l;
}


cstring_t cstring_from_longlong(long long value)
{
    char buf[CSTRING_LONGLLONG_STRING_SIZE];
    int len = lltostr(buf, value);
    return cstring_new_n(buf, len);
}


cstring_t cstring_from_ulonglong(unsigned long long value, int base)
{
    char buf[CSTRING_LONGLLONG_STRING_SIZE];
    size_t len = xulltostr(buf, value, base);
    return cstring_new_n(buf, len);
}


cstring_t cstring_cat_vprintf(cstring_t cs, const char *fmt, va_list ap)
{
    va_list cpy;
    char staticbuf[1024], *buf = staticbuf, *t;
    size_t buflen = strlen(fmt) * 2;

    if (buflen > sizeof(staticbuf)) {
        buf = xmalloc(buflen);
        if (buf == NULL) return NULL;
    } else {
        buflen = sizeof(staticbuf);
    }

    for(;;) {
        buf[buflen - 2] = '\0';
        va_copy(cpy, ap);
        vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);
        if (buf[buflen - 2] != '\0') {
            if (buf != staticbuf) xfree(buf);
            buflen *= 2;
            buf = xmalloc(buflen);
            if (buf == NULL) return NULL;
            continue;
        }
        break;
    }

    t = cstring_cat(cs, buf);
    if (buf != staticbuf) {
        xfree(buf);
    }

    return t;
}


cstring_t cstring_cat_printf(cstring_t cs, const char *fmt, ...)
{
    va_list ap;
    char *t;
    va_start(ap, fmt);
    t = cstring_cat_vprintf(cs, fmt, ap);
    va_end(ap);
    return t;
}


cstring_t cstring_catfmt(cstring_t cs, char const *fmt, ...) {
    size_t initlen = cstring_length(cs);
    const char *f = fmt;
    int i;
    va_list ap;

    va_start(ap, fmt);
    f = fmt;
    i = initlen;
    while (*f) {
        char next, *str;
        size_t l;
        long long num;
        unsigned long long unum;

        if (cstring_avail(cs) == 0) {
            cs = cstring_make_room_for(cs, 1);
        }

        switch (*f) {
        case '%':
            next = *(f + 1);
            f++;
            switch (next) {
            case 's':
            case 'S':
                str = va_arg(ap, char*);
                l = (next == 's') ? strlen(str) : cstring_length(str);
                if (cstring_avail(cs) < l) {
                    cs = cstring_make_room_for(cs, l);
                }
                memcpy(cs + i, str, l);
                cstring_increase_length(cs, l);
                i += l;
                break;
            case 'i':
            case 'I':
                if (next == 'i') {
                    num = va_arg(ap, int);
                } else {
                    num = va_arg(ap, long long);
                }
                {
                    char buf[CSTRING_LONGLLONG_STRING_SIZE];
                    l = lltostr(buf, num);
                    if (cstring_avail(cs) < l) {
                        cs = cstring_make_room_for(cs, l);
                    }
                    memcpy(cs + i, buf, l);
                    cstring_increase_length(cs, l);
                    i += l;
                }
                break;
            case 'u':
            case 'U':
                if (next == 'u') {
                    unum = va_arg(ap, unsigned int);
                } else {
                    unum = va_arg(ap, unsigned long long);
                }
                {
                    char buf[CSTRING_LONGLLONG_STRING_SIZE];
                    l = ulltostr(buf, unum);
                    if (cstring_avail(cs) < l) {
                        cs = cstring_make_room_for(cs, l);
                    }
                    memcpy(cs + i, buf, l);
                    cstring_increase_length(cs, l);
                    i += l;
                }
                break;
            default:
                cs[i++] = next;
                cstring_increase_length(cs, 1);
                break;
            }
            break;
        default:
            cs[i++] = *f;
            cstring_increase_length(cs, 1);
            break;
        }
        f++;
    }

    va_end(ap);
    cs[i] = '\0';
    return cs;
}


cstring_t cstring_trim(cstring_t cs, const char *cset)
{
    char *start, *end, *sp, *ep;
    size_t len;

    sp = start = cs;
    ep = end = cs + cstring_length(cs) - 1;
    while (sp <= end && strchr(cset, *sp)) sp++;
    while (ep > sp && strchr(cset, *ep)) ep--;
    len = (sp > ep) ? 0 : ((ep - sp) + 1);
    if (cs != sp) memmove(cs, sp, len);
    cs[len] = '\0';
    cstring_set_length(cs, len);
    return cs;
}


cstring_t cstring_exclude(cstring_t cs, const char *cset)
{
    char *cp, *sp, *ep;

    sp = cp = cs;
    ep = cs + cstring_length(cs) - 1;

    while (sp <= ep) {
        if (!strchr(cset, *sp)) {
            *cp++ = *sp++;
        } else {
            sp++;
        }
    }

    *cp = '\0';

    cstring_update_length(cs);
    return cs;
}


void cstring_range(cstring_t cs, int start, int end)
{
    size_t newlen, len = cstring_length(cs);

    if (len == 0) {
        return;
    }

    if (start < 0) {
        start = len + start;
        if (start < 0) {
            start = 0;
        }
    }

    if (end < 0) {
        end = len + end;
        if (end < 0) {
            end = 0;
        }
    }

    newlen = (start > end) ? 0 : (end - start) + 1;
    if (newlen != 0) {
        if (start >= (int)len) {
            newlen = 0;
        } else if (end >= (int)len) {
            end = len - 1;
            newlen = (start > end) ? 0 : (end - start) + 1;
        }
    } else {
        start = 0;
    }
    
    if (start && newlen) {
        memmove(cs, cs + start, newlen);
    }

    cs[newlen] = 0;
    cstring_set_length(cs, newlen);
}


void cstring_tolower(cstring_t cs)
{
    int len = cstring_length(cs), j;

    for (j = 0; j < len; j++) cs[j] = tolower(cs[j]);
}


void cstring_toupper(cstring_t cs)
{
    int len = cstring_length(cs), j;

    for (j = 0; j < len; j++) cs[j] = toupper(cs[j]);
}


int cstring_compare(const cstring_t s1, const cstring_t s2)
{
    size_t l1, l2, minlen;
    int cmp;

    l1 = cstring_length(s1);
    l2 = cstring_length(s2);
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s1, s2, minlen);
    if (cmp == 0) return l1 - l2;
    return cmp;
}


cstring_t *cstring_splinters(const char *s, int len, const char *sep, int seplen, int *count)
{
    int elements = 0, slots = 5, start = 0, j;
    cstring_t *tokens;

    if (seplen < 1 || len < 0) return NULL;

    tokens = xmalloc(sizeof(cstring_t)*slots);
    if (tokens == NULL) return NULL;

    if (len == 0) {
        *count = 0;
        return tokens;
    }

    for (j = 0; j < (len - (seplen - 1)); j++) {
        if (slots < elements + 2) {
            cstring_t *newtokens;
            slots *= 2;
            newtokens = xrealloc(tokens, sizeof(cstring_t)*slots);
            if (newtokens == NULL) goto cleanup;
            tokens = newtokens;
        }
        if ((seplen == 1 && *(s + j) == sep[0]) || (memcmp(s + j, sep, seplen) == 0)) {
            tokens[elements] = cstring_new_n(s + start, j - start);
            if (tokens[elements] == NULL) goto cleanup;
            elements++;
            start = j + seplen;
            j = j + seplen - 1;
        }
    }

    tokens[elements] = cstring_new_n(s + start, len - start);
    if (tokens[elements] == NULL) goto cleanup;
    elements++;
    *count = elements;
    return tokens;

cleanup:
    {
        int i;
        for (i = 0; i < elements; i++) {
            cstring_free(tokens[i]);
        }
        xfree(tokens);
        *count = 0;
        return NULL;
    }
}


void cstring_free_splinters(cstring_t *tokens, int count)
{
    if (!tokens) return;
    while (count--)
        cstring_free(tokens[count]);
    xfree(tokens);
}


cstring_t cstring_cat_repr(cstring_t cs, const char *s, size_t len)
{
    cs = cstring_cat_n(cs, "\"", 1);
    while (len--) {
        switch (*s) {
        case '\\':
        case '"':
            cs = cstring_cat_printf(cs, "\\%c", *s);
            break;
        case '\n': cs = cstring_cat_n(cs, "\\n", 2); break;
        case '\r': cs = cstring_cat_n(cs, "\\r", 2); break;
        case '\t': cs = cstring_cat_n(cs, "\\t", 2); break;
        case '\a': cs = cstring_cat_n(cs, "\\a", 2); break;
        case '\b': cs = cstring_cat_n(cs, "\\b", 2); break;
        default:
            if (isprint(*s)) {
                cs = cstring_cat_printf(cs, "%c", *s);
            } else {
                cs = cstring_cat_printf(cs, "\\x%02x", (unsigned char)*s);
            }
            break;
        }
        s++;
    }
    return cstring_cat_n(cs, "\"", 1);
}


static inline
int is_hex_digit(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F');
}


static inline
int hex_digit_to_int(char c)
{
    switch (c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
    }
}


cstring_t *cstring_split_args(const char *line, int *argc)
{
    const char *p = line;
    char *current = NULL;
    char **vector = NULL;

    *argc = 0;
    for(;;) {
        while (*p && isspace(*p)) p++;
        if (*p) {
            int inq = 0;
            int insq = 0;
            int done = 0;

            if (current == NULL) current = cstring_empty();
            while (!done) {
                if (inq) {
                    if (*p == '\\' && *(p + 1) == 'x' &&
                        is_hex_digit(*(p + 2)) &&
                        is_hex_digit(*(p + 3))) {
                        unsigned char byte;

                        byte = (hex_digit_to_int(*(p + 2)) * 16) + hex_digit_to_int(*(p + 3));
                        current = cstring_cat_n(current, (char*)&byte, 1);
                        p += 3;
                    } else if (*p == '\\' && *(p + 1)) {
                        char c;

                        p++;
                        switch (*p) {
                        case 'n': c = '\n'; break;
                        case 'r': c = '\r'; break;
                        case 't': c = '\t'; break;
                        case 'b': c = '\b'; break;
                        case 'a': c = '\a'; break;
                        default: c = *p; break;
                        }
                        current = cstring_cat_n(current, &c, 1);
                    } else if (*p == '"') {
                        if (*(p + 1) && !isspace(*(p + 1))) goto err;
                        done = 1;
                    } else if (!*p) {
                        goto err;
                    } else {
                        current = cstring_cat_n(current, p, 1);
                    }
                } else if (insq) {
                    if (*p == '\\' && *(p + 1) == '\'') {
                        p++;
                        current = cstring_cat_n(current, "'", 1);
                    } else if (*p == '\'') {
                        if (*(p + 1) && !isspace(*(p + 1))) goto err;
                        done = 1;
                    } else if (!*p) {
                        goto err;
                    } else {
                        current = cstring_cat_n(current, p, 1);
                    }
                } else {
                    switch (*p) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                    case '\0':
                        done = 1;
                        break;
                    case '"':
                        inq = 1;
                        break;
                    case '\'':
                        insq = 1;
                        break;
                    default:
                        current = cstring_cat_n(current, p, 1);
                        break;
                    }
                }
                if (*p) p++;
            }
            
            vector = xrealloc(vector, ((*argc) + 1) * sizeof(char*));
            vector[*argc] = current;
            (*argc)++;
            current = NULL;
        } else {
            if (vector == NULL) {
                vector = xmalloc(sizeof(void*));
            }
            return vector;
        }
    }

err:
    if (current) {
        cstring_free(current);
    }

    while ((*argc)--) {
        cstring_free(vector[*argc]);
    }
    xfree(vector);

    *argc = 0;
    return NULL;
}


cstring_t cstring_map_chars(cstring_t cs, const char *from, const char *to, size_t setlen)
{
    size_t j, i, l = cstring_length(cs);

    for (j = 0; j < l; j++) {
        for (i = 0; i < setlen; i++) {
            if (cs[j] == from[i]) {
                cs[j] = to[i];
                break;
            }
        }
    }
    return cs;
}


cstring_t cstring_join(char **argv, int argc, char *sep)
{
    cstring_t join = cstring_empty();
    int j;

    for (j = 0; j < argc; j++) {
        join = cstring_cat(join, argv[j]);
        if (j != argc - 1) {
            join = cstring_cat(join, sep);
        }
    }
    return join;
}


cstring_t cstring_join_cs(cstring_t *argv, int argc, const char *sep, size_t sep_n)
{
    cstring_t join = cstring_empty();
    int j;

    for (j = 0; j < argc; j++) {
        join = cstring_cat_cs(join, argv[j]);
        if (j != argc - 1) {
            join = cstring_cat_n(join, sep, sep_n);
        }
    }
    return join;
}
