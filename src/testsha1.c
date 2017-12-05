

#include "unittest.h"
#include "sha1.h"


static
void testsha1()
{
    uint8_t result[20];
    uint8_t Helloworld[20] = { 0x1c, 0x3c, 0x3f, 0xa0, 0xa3, 0x2a, 0xbf, 0x34, 0x73, 0xa3, 0xe8, 0x8f, 0x07, 0xa3, 0x77, 0x02, 0x5e, 0x28, 0xc0, 0x3e };
    uint8_t helloworld[20] = { 0x6a, 0xdf, 0xb1, 0x83, 0xa4, 0xa2, 0xc9, 0x4a, 0x2f, 0x92, 0xda, 0xb5, 0xad, 0xe7, 0x62, 0xa4, 0x78, 0x89, 0xa5, 0xa1 };

    sha1_fast("Helloworld", 10, result);
    TEST_COND("SHA1('Helloworld'): ", memcmp(result, Helloworld, 20) == 0);

    sha1_fast("helloworld", 10, result);
    TEST_COND("SHA1('helloworld'): ", memcmp(result, helloworld, 20) == 0);
}


int main(void)
{
#if defined(WINDOWS) && defined(DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

    testsha1();
    TEST_REPORT();
    return 0;
}
