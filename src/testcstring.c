

#include <limits.h>

#include "config.h"
#include "cstring.h"
#include "unittest.h"


static
void testcstring1(void)
{
    cstring_t x = cstring_new("foo"), y;

    TEST_COND("Create a string and obtain the length",
              cstring_length(x) == 3 && memcmp(x, "foo\0", 4) == 0);

    cstring_free(x);
    x = cstring_new_n("foo", 2);
    TEST_COND("Create a string with specified length",
              cstring_length(x) == 2 && memcmp(x, "fo\0", 3) == 0);

    x = cstring_cat(x, "bar");
    TEST_COND("Strings concatenation",
              cstring_length(x) == 5 && memcmp(x, "fobar\0", 6) == 0);

    x = cstring_copy(x, "a");
    TEST_COND("cstring_copy() against an originally longer string",
              cstring_length(x) == 1 && memcmp(x, "a\0", 2) == 0);

    x = cstring_copy(x, "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    TEST_COND("cstring_copy() against an originally shorter string",
              cstring_length(x) == 33 &&
              memcmp(x, "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0", 33) == 0);

    cstring_free(x);
    x = cstring_cat_printf(cstring_empty(), "%d", 123);
    TEST_COND("cstring_cat_printf() seems working in the base case",
              cstring_length(x) == 3 && memcmp(x, "123\0", 4) == 0);

    cstring_free(x);
    x = cstring_new("--");
    x = cstring_catfmt(x, "Hello %s World %I,%I--", "Hi!", LLONG_MIN, LLONG_MAX);
    TEST_COND("cstring_catfmt() seems working in the base case",
              cstring_length(x) == 60 &&
              memcmp(x, "--Hello Hi! World -9223372036854775808,"
                      "9223372036854775807--", 60) == 0);
    printf("[%s]\n", x);

    cstring_free(x);
    x = cstring_new("--");
    x = cstring_catfmt(x, "%u,%U--", UINT_MAX, ULLONG_MAX);
    TEST_COND("cstring_catfmt() seems working with unsigned numbers",
              cstring_length(x) == 35 &&
              memcmp(x, "--4294967295,18446744073709551615--", 35) == 0);

    cstring_free(x);
    x = cstring_new(" x ");
    cstring_trim(x, " x");
    TEST_COND("cstring_trim() works when all chars match",
              cstring_length(x) == 0);

    cstring_free(x);
    x = cstring_new(" x ");
    cstring_trim(x, " ");
    TEST_COND("cstring_trim() works when a single char remains",
              cstring_length(x) == 1 && x[0] == 'x');

    cstring_free(x);
    x = cstring_new("xxciaoyyy");
    cstring_trim(x, "xy");
    TEST_COND("cstring_trim() correctly trims characters",
              cstring_length(x) == 4 && memcmp(x, "ciao\0", 5) == 0);

    y = cstring_dup(x);
    cstring_range(y, 1, 1);
    TEST_COND("cstring_range(...,1,1)",
              cstring_length(y) == 1 && memcmp(y, "i\0", 2) == 0);

    cstring_free(y);
    y = cstring_dup(x);
    cstring_range(y, 1, -1);
    y = cstring_shrink_to_fit(y);
    TEST_COND("cstring_shrink_to_fit(...,1,-1)",
              cstring_length(y) == 3 && memcmp(y, "iao\0", 4) == 0);

    cstring_free(y);
    y = cstring_dup(x);
    cstring_range(y, -2, -1);
    TEST_COND("cstring_range(...,-2,-1)",
              cstring_length(y) == 2 && memcmp(y, "ao\0", 3) == 0);

    cstring_free(y);
    y = cstring_dup(x);
    cstring_range(y, 2, 1);
    TEST_COND("cstring_range(...,2,1)",
              cstring_length(y) == 0 && memcmp(y, "\0", 1) == 0);

    cstring_free(y);
    y = cstring_dup(x);
    cstring_range(y, 1, 100);
    TEST_COND("cstring_range(...,1,100)",
              cstring_length(y) == 3 && memcmp(y, "iao\0", 4) == 0);

    cstring_free(y);
    y = cstring_dup(x);
    cstring_range(y, 100, 100);
    TEST_COND("cstring_range(...,100,100)",
              cstring_length(y) == 0 && memcmp(y, "\0", 1) == 0);

    cstring_free(y);
    cstring_free(x);
    x = cstring_new("foo");
    y = cstring_new("foa");
    TEST_COND("cstring_compare(foo,foa)", cstring_compare(x, y) > 0);

    cstring_free(y);
    cstring_free(x);
    x = cstring_new("bar");
    y = cstring_new("bar");
    TEST_COND("cstring_compare(bar,bar)", cstring_compare(x, y) == 0);

    cstring_free(y);
    cstring_free(x);
    x = cstring_new("aar");
    y = cstring_new("bar");
    TEST_COND("cstring_compare(bar,bar)", cstring_compare(x, y) < 0);

    cstring_free(y);
    cstring_free(x);
    x = cstring_new_n("\a\n\0foo\r", 7);
    y = cstring_cat_repr(cstring_empty(), x, cstring_length(x));
    TEST_COND("cstring_cat_repr(...data...)",
              memcmp(y, "\"\\a\\n\\x00foo\\r\"", 15) == 0);

    {
        char *p;
        int step = 10, j, i;

        cstring_free(x);
        cstring_free(y);
        x = cstring_new("0");
        TEST_COND("cstring_new() free/len buffers", cstring_length(x) == 1 && cstring_avail(x) == 0);

        for (i = 0; i < 10; i++) {
            int oldlen = cstring_length(x);
            x = cstring_make_room_for(x, step);
            int type = x[-1] & CSTRING_TYPE_MASK;

            TEST_COND("cstring_make_room_for() len", cstring_length(x) == oldlen);
            if (type != CSTRING_TYPE_5) {
                unsigned int oldfree;
                TEST_COND("cstring_make_room_for() free", cstring_avail(x) >= step);
                oldfree = cstring_avail(x);
            }
            p = x + oldlen;
            for (j = 0; j < step; j++) {
                p[j] = 'A' + j;
            }
            cstring_increase_string_length(x, step);
        }
        TEST_COND("cstring_make_room_for() content",
                  memcmp("0ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ", x, 101) == 0);
        TEST_COND("cstring_make_room_for() final length", cstring_length(x) == 101);

        cstring_free(x);
    }
}

static
void testcstring2(void)
{
    cstring_t cs, cs2;

    cs = cstring_new("");
    TEST_COND("cstring_length()", cstring_length(cs) == 0);
    cstring_free(cs);

    cs = cstring_from_longlong(-1);
    TEST_COND("cstring_from_ll()", memcmp(cs, "-1", 3) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 2);
    cstring_free(cs);

    cs = cstring_from_longlong(0x7fffffffffffffffl);
    TEST_COND("cstring_from_ll()", memcmp(cs, "9223372036854775807", 20) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 19);
    cstring_free(cs);

    cs = cstring_from_ulonglong(-1, 10);
    TEST_COND("cstring_from_ulonglong()", memcmp(cs, "18446744073709551615", 21) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 20);
    cstring_free(cs);


    cs = cstring_new_n("", 0);
    cs = cstring_cat_printf(cs, "%d, %lf, %s", 1024, 1.234, "abcd");
    TEST_COND("cstring_cat_printf()", memcmp(cs, "1024, 1.234000, abcd", 20) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 20);
    cstring_free(cs);

    cs = cstring_new("AA...AA.a.aa.aHelloWorld     :::");
    cs = cstring_trim(cs, "Aa. :");
    TEST_COND("cstring_trim()", memcmp(cs, "HelloWorld", 10) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 10);
    cstring_free(cs);

    cs = cstring_new("AA...AA.a.aa.aHe:llo World     :::");
    cs = cstring_exclude(cs, "Aa. :");
    TEST_COND("cstring_exclude()", memcmp(cs, "HelloWorld", 10) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 10);
    cstring_free(cs);

    cs = cstring_new("HelloWorld");
    cs2 = cstring_dup(cs);
    TEST_COND("cstring_dup()", cstring_compare(cs, cs2) == 0);
    TEST_COND("cstring_length()", cstring_length(cs) == 10);
    TEST_COND("cstring_length()", cstring_length(cs2) == 10);
    cstring_tolower(cs);
    TEST_COND("cstring_tolower()", memcmp(cs, "helloworld", 10) == 0);
    cstring_toupper(cs);
    TEST_COND("cstring_toupper()", memcmp(cs, "HELLOWORLD", 10) == 0);
    cstring_free(cs);
    cstring_free(cs2);
}

int main(void)
{
#if defined(WINDOWS) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    testcstring1();
    testcstring2();

    TEST_REPORT();
    return 0;
}
