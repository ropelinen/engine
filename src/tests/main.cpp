#include "tests/config/precompiled.h"

#include <stdlib.h>

#include "tests/tests/vector_tests.h"

DEA_START()

void run_tests()
{
	bool total_success = true;

	START_TEST_CATEGORY("POD VECTOR");
	RUN_TEST_CATEGORY(test_pod_vector, "POD VECTOR", total_success);
	END_TEST_CATEGORY();

	START_TEST_CATEGORY("POD VECTOR ITERATOR");
	RUN_TEST_CATEGORY(test_pod_vector_iterator, "POD VECTOR ITERATOR", total_success);
	END_TEST_CATEGORY();

	START_TEST_CATEGORY("VECTOR");
	RUN_TEST_CATEGORY(test_vector, "VECTOR", total_success);
	END_TEST_CATEGORY();

	START_TEST_CATEGORY("VECTOR ITERATOR");
	RUN_TEST_CATEGORY(test_vector_iterator, "VECTOR ITERATOR", total_success);
	END_TEST_CATEGORY();

	END_TEST_CATEGORY();
	if (total_success)
		printf("ALL TEST SUCCEEDED!\n");
	else
		printf("TEST FAILED\n");
}

DEA_END()

int main(char **argv, int argc)
{
	/* Just to get this to compile */
	(void *)argv;
	--argc;

	dea::run_tests();

	system("PAUSE");

	return 0;
}
