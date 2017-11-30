

#ifndef __UNITTEST__H__INCLUDE__
#define __UNITTEST__H__INCLUDE__


#include <stdio.h>


static int __unittest_failed_tests__ = 0;
static int __unittest_test_num__ = 0;


#define TEST_COND(desc, cond)                                                                   \
    do {                                                                                        \
        __unittest_test_num__++;                                                                \
        printf("%d - %s: ", __unittest_test_num__, desc);                                       \
        if (cond) {                                                                             \
            printf("PASSED\n");                                                                 \
        } else {                                                                                \
            printf("FAILED\n");                                                                 \
            __unittest_failed_tests__++;                                                        \
        }                                                                                       \
    } while(0)


#define TEST_REPORT()                                                                           \
    do {                                                                                        \
        printf("====== TEST REPORT ======\n%d tests, %d passed, %d failed\n",                   \
               __unittest_test_num__, __unittest_test_num__ - __unittest_failed_tests__,        \
               __unittest_failed_tests__);                                                      \
    } while(0)


#endif
