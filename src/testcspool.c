

#include "unittest.h"
#include "cspool.h"
#include <malloc.h>

static
void testcspool(void)
{
    cspool_t csp;
    cstring_t cs;

    cspool_init(&csp);

    cs = cspool_get(&csp, "HelloWorld", 10);
    TEST_COND("cspool_get()", memcmp(cs, "HelloWorld", 10) == 0);
    cs = cspool_get(&csp, "HelloWorld", 10);
    TEST_COND("cspool_get()", memcmp(cs, "HelloWorld", 10) == 0);
    cspool_uninit(&csp);
}


int main(void)
{
#if defined(WINDOWS) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    testcspool();

    TEST_REPORT();
    return 0;
}
