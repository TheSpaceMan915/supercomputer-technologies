#include "unity.h"
#include <stdio.h>

static int g_failures = 0;
static int g_ran = 0;
void UnityBegin(const char* name) { (void)name; g_failures=0; g_ran=0; }
void UnityEnd(void) {
  if (g_failures==0) printf("OK (%d tests)\n", g_ran);
  else printf("FAIL (%d failures / %d tests)\n", g_failures, g_ran);
}
void RUN_TEST(void (*test)(void), const char* name) { (void)name; ++g_ran; test(); }
void UnityAssertEqualInt(int expected, int actual, const char* msg) {
  if (expected!=actual) { ++g_failures; printf("Assertion failed: %s (exp=%d act=%d)\n", msg, expected, actual); }
}
