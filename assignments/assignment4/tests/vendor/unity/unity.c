#include "unity.h"

int UnityTestCount = 0;
int UnityFailCount = 0;

int UnityBegin(const char* name)
{
    UnityTestCount = 0;
    UnityFailCount = 0;
    printf("==== Unity: %s ====\n", name ? name : "(unnamed)");
    return 0;
}

void UnityDefaultTestRun(UnityTestFunction test, const char* name, int line)
{
    (void)line;
    printf("[ RUN      ] %s\n", name ? name : "(anonymous)");
    test();
    if (UnityFailCount == 0)
        printf("[       OK ] %s\n", name ? name : "(anonymous)");
    else
        printf("[  FAILED  ] %s\n", name ? name : "(anonymous)");
}

int UnityEnd(void)
{
    if (UnityFailCount == 0)
        printf("==== ALL TESTS PASSED (%d assertions) ====\n", UnityTestCount);
    else
        printf("==== %d FAILURE(S) / %d assertion(s) ====\n",
               UnityFailCount, UnityTestCount);
    return UnityFailCount;
}
