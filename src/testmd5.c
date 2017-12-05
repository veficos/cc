

#include "unittest.h"
#include "md5.h"


static
void testmd5()
{
    uint8_t result[16];
    uint8_t Helloworld[16] = { 0xa1, 0x65, 0x96, 0x8b, 0x0a, 0x80, 0x84, 0xa0, 0x41, 0xae, 0xd8, 0x9b, 0xf4, 0x0d, 0x58, 0x1f };
    uint8_t helloworld[16] = { 0xfc, 0x5e, 0x03, 0x8d, 0x38, 0xa5, 0x70, 0x32, 0x08, 0x54, 0x41, 0xe7, 0xfe, 0x70, 0x10, 0xb0 };

    md5_fast("Helloworld", 10, result);
    TEST_COND("MD5('Helloworld'): ", memcmp(result, Helloworld, 16) == 0);

    md5_fast("helloworld", 10, result);
    TEST_COND("MD5('helloworld'): ", memcmp(result, helloworld, 16) == 0);
}


int main(void)
{
#if defined(WINDOWS) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    testmd5();
    TEST_REPORT();
    return 0;
}
