

#include "unittest.h"
#include "array.h"


static 
void test_array(void)
{
    array_t *array;
    int *a;
    int i;
    
    array = array_create(sizeof(int));

    TEST_COND("array_is_empty()", array_is_empty(array));
    TEST_COND("array_length()", array_length(array) == 0);
    TEST_COND("array_capacity()", array_capacity(array) == 0);

    for (i = 0; i < 10; i++) {
        a = array_push_back(array);
        *a = i;
    }

    for (i = 0; i < 10; i++) {
        TEST_COND("array_cast_at()", array_cast_at(int, array, i) == i);
    }
    
    TEST_COND("array_cast_front", array_cast_front(int, array) == 0);
    TEST_COND("array_cast_back", array_cast_back(int, array) == 9);
    TEST_COND("array_is_empty()", !array_is_empty(array));
    TEST_COND("array_length()", array_length(array) == 10);
    TEST_COND("array_capacity()", array_capacity(array) == 6);

    array_foreach(array, a, i) {
        TEST_COND("array_foreach()", a == array->elts);
        TEST_COND("array_push()", a[i] == i);
    }

    array_pop_back(array);
    TEST_COND("array_pop()", array_length(array) == 9);

    array_pop_back_n(array, 2);
    TEST_COND("array_pop_n()", array_length(array) == 7);

    array_clear(array);
    TEST_COND("array_is_empty()", array_is_empty(array));
    TEST_COND("array_capacity()", array_capacity(array) == 16);

    array_destroy(array);
}


int main(void)
{
#if defined(WINDOWS) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    test_array();
    TEST_REPORT();
    return 0;
}
