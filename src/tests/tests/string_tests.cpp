#include "tests/config/precompiled.h"
#include "tests/tests/string_tests.h"

#include "core/container/string.h"

#include <cstring>

DEA_START()

bool test_string()
{
	bool result = true;

	string test_string;
	RUN_TEST(strcmp(test_string.c_str(), "") == 0, "Constructor test 1", result);
	
	const char *str = "This is a test string";
	string test_string2(str);
	RUN_TEST(strcmp(test_string2.c_str(), str) == 0, "Constructor test 2", result);
	
	string test_string3(test_string2);
	RUN_TEST(strcmp(test_string2.c_str(), test_string3.c_str()) == 0, "Constructor test 3", result);

	test_string = test_string2;
	RUN_TEST(strcmp(test_string.c_str(), test_string2.c_str()) == 0, "Assignment test 1", result);

	string test_string4;
	test_string4 = string(str);
	RUN_TEST(strcmp(test_string.c_str(), test_string4.c_str()) == 0, "Assignment test 2", result);

	const char *str2 = "This is short str";
	string test_string5;
	test_string5 = str;
	RUN_TEST(strcmp(test_string5.c_str(), str) == 0, "Assignment test 3", result);

	test_string5 = str2;
	RUN_TEST(strcmp(test_string5.c_str(), str2) == 0, "Assignment test 4", result);

	RUN_TEST(test_string == test_string2, "Equals test 1", result);
	RUN_TEST(test_string == str, "Equals test 2", result);
	
	return result;
}

DEA_END()
