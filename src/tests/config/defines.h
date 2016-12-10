#ifndef DEA_TESTS_DEFINES_H
#define DEA_TESTS_DEFINES_H

#include <stdio.h>

DEA_START()

#define START_TEST_CATEGORY(category) printf(category "\n")

#define RUN_TEST_CATEGORY(test_func, test_name) \
if (test_func()) \
	printf("SUCCESS: " test_name "\n"); \
else \
	printf("FAIL: " test_name "\n")

#define RUN_TEST(test, test_name, success_bool) \
if (!(test)) \
{ \
	printf("- Fail: " test_name "\n"); \
	success_bool = false; \
} \
else \
	printf("- Success: " test_name "\n")

#define END_TEST_CATEGORY() printf("-------------------------------\n")

DEA_END()

#endif // DEA_TESTS_DEFINES_H