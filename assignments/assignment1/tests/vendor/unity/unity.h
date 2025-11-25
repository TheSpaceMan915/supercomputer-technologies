#ifndef UNITY_MINI_H
#define UNITY_MINI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef void (*UnityTestFunction)(void);

int  UnityBegin(const char* name);
int  UnityEnd(void);
void UnityDefaultTestRun(UnityTestFunction test, const char* name, int line);

extern int UnityTestCount;
extern int UnityFailCount;

#define TEST_ASSERT_TRUE(cond) do {     if (!(cond)) {         printf("Assertion failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__);         ++UnityFailCount;     }     ++UnityTestCount; } while(0)

#define TEST_ASSERT_LESS_THAN_DOUBLE(threshold, value) do {     if (!((value) < (threshold))) {         printf("Assertion failed: %s < %s (%s:%d)\n", #value, #threshold, __FILE__, __LINE__);         ++UnityFailCount;     }     ++UnityTestCount; } while(0)

#define RUN_TEST(fn) UnityDefaultTestRun((fn), #fn, __LINE__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UNITY_MINI_H */
