#ifndef UNITY_MINI_H
#define UNITY_MINI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef void (*UnityTestFunction)(void);

int  UnityBegin(const char* name);
void UnityDefaultTestRun(UnityTestFunction test, const char* name, int line);
int  UnityEnd(void);

extern int UnityTestCount;
extern int UnityFailCount;

#define TEST_ASSERT_TRUE(cond) do {                           \
    if (!(cond)) {                                            \
        printf("Assertion failed: %s (%s:%d)\n",              \
               #cond, __FILE__, __LINE__);                    \
        ++UnityFailCount;                                     \
    }                                                         \
    ++UnityTestCount;                                         \
} while (0)

#define TEST_ASSERT_DOUBLE_WITHIN(eps, expected, actual) do { \
    double diff_ = ((actual) > (expected)) ?                  \
        ((actual) - (expected)) : ((expected) - (actual));    \
    if (!(diff_ <= (eps))) {                                  \
        printf("Assertion failed: |%s - %s| <= %s (%s:%d)\n", \
               #actual, #expected, #eps, __FILE__, __LINE__); \
        ++UnityFailCount;                                     \
    }                                                         \
    ++UnityTestCount;                                         \
} while (0)

#define RUN_TEST(fn) UnityDefaultTestRun((fn), #fn, __LINE__)

#ifdef __cplusplus
}
#endif

#endif
