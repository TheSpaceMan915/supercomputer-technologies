#ifndef UNITY_H
#define UNITY_H
#ifdef __cplusplus
extern "C" {
#endif
void UnityBegin(const char* name);
void UnityEnd(void);
void RUN_TEST(void (*test)(void), const char* name);
void UnityAssertEqualInt(int expected, int actual, const char* msg);
#ifdef __cplusplus
}
#endif
#endif
