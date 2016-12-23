#include "tests/config/precompiled.h"
#include "tests/tests/vector_tests.h"

#include "core/container/pod_vector.h"
#include "core/container/vector.h"

DEA_START()

class non_pod
{
public:
	non_pod() = delete;
	non_pod(int val) { data = new int; *data = val; }
	non_pod(const non_pod &other) { data = new int; *data = *(other.data); }

	~non_pod() { delete data; data = 0; }

	non_pod &operator=(const non_pod& other) { *data = *(other.data); return *this; }
	non_pod &operator=(const non_pod&& other) { *data = *(other.data); return *this; }

	bool operator==(const non_pod &other) const { return *data == *other.data; }

	int get_data() { return *data; }

private:
	int *data;
};

/* POD */
bool test_pod_vector()
{
	bool result = true;

	/* Should trigger static assert */
	//pod_vector<non_pod(1)> should_assert;

	const uint start_capacity = 8;
	pod_vector<int> test_pod(start_capacity);
	
	RUN_TEST(test_pod.empty(), "Empty test 1", result);
	RUN_TEST(test_pod.get_capacity() == start_capacity, "Capacity test 1", result);
	RUN_TEST(test_pod.get_size() == 0, "Size test 1", result);

	/* Should assert */
	//test_pod.pop_back();

	/* push_back tests */
	test_pod.push_back(1);
	test_pod.push_back(2);

	RUN_TEST(!test_pod.empty(), "Empty test 2", result);
	RUN_TEST(test_pod.get_size() == 2, "Size test 2", result);

	/* Indexing tests */
	int first = test_pod[0];
	RUN_TEST(first == 1, "Get at index test 1", result);
	const int second = test_pod[1];
	RUN_TEST(second == 2, "Get at index test 2", result);

	/* Should assert */
	//test_pod[4];

	/* Automatic capacity increase */
	for (int i = 3; i < start_capacity + 2; ++i)
		test_pod.push_back(i);

	RUN_TEST(test_pod.get_capacity() > start_capacity, "Capacity test 2", result);
	RUN_TEST(test_pod.get_size() == start_capacity + 1, "Size test 3", result);
	RUN_TEST(test_pod[test_pod.get_size()-1] == start_capacity + 1, "Get at index test 3", result);

	/* pop_back testing */
	const uint size_after_push = test_pod.get_size();
	test_pod.pop_back();
	RUN_TEST(test_pod.get_size() == size_after_push - 1, "Size test 4", result);

	/* swap_out testing */
	test_pod.swap_out(0);
	RUN_TEST(test_pod[0] == start_capacity, "Get at index test 4", result);
	RUN_TEST(test_pod.get_size() == size_after_push - 2, "Size test 5", result);

	/* trim testing */
	test_pod.trim();
	RUN_TEST(test_pod.get_size() == test_pod.get_capacity(), "Trim test 1", result);

	/* remove testing */
	int third = test_pod[2];
	test_pod.remove(1);
	RUN_TEST(test_pod[1] == third, "Erase test 1", result);
	uint size_temp = test_pod.get_size();
	test_pod.remove(size_temp - 1);
	RUN_TEST(test_pod.get_size() == size_temp - 1, "Erase test 2", result);

	/* insert testing */
	uint size_before_insert = test_pod.get_size();
	first = test_pod[0];
	test_pod.insert(0, 16);
	RUN_TEST(test_pod[0] == 16, "Insert test 1", result);
	RUN_TEST(test_pod[1] == first, "Insert test 2", result);
	RUN_TEST(test_pod.get_size() == size_before_insert + 1, "Insert test 3", result);
	int last = test_pod[test_pod.get_size() - 1];
	test_pod.insert(test_pod.get_size() - 1, -123);
	RUN_TEST(test_pod[test_pod.get_size() - 2] == -123, "Insert test 4", result);
	RUN_TEST(test_pod[test_pod.get_size() - 1] == last, "Insert test 5", result);

	/* reserve testing */
	uint current_capacity = test_pod.get_capacity();
	test_pod.reserve(current_capacity * 2);
	RUN_TEST(test_pod.get_capacity() == current_capacity * 2, "Reserve test 1", result);
	RUN_TEST(test_pod[0] == 16, "Reserve test 2", result);
	RUN_TEST(test_pod[1] == first, "Reserve test 3", result);
	RUN_TEST(test_pod[test_pod.get_size() - 2] == -123, "Reserve test 4", result);
	RUN_TEST(test_pod[test_pod.get_size() - 1] == last, "Reserve test 5", result);

	/* clear */
	test_pod.clear();

	RUN_TEST(test_pod.empty(), "Empty test 3", result);
	RUN_TEST(test_pod.get_size() == 0, "Size test 6", result);

	/* Should fail */
	//RUN_TEST(test_pod.get_size() != 0, "Test test 1", result);

	/* resize */
	const uint test_arr_size = 5;
	const int resize_default_val = 0;
	int test_arr[test_arr_size] = { 45, -123, 63, -6132, 12 };
	for (uint i = 0; i < test_arr_size; ++i)
		test_pod.push_back(test_arr[i]);

	test_pod.resize(test_arr_size + 2, resize_default_val);
	for (uint i = 0; i < test_arr_size; ++i)
	{
		RUN_TEST(test_pod[i] == test_arr[i], "Resize test 1", result);
	}
	for (uint i = test_arr_size; i < test_arr_size + 2; ++i)
	{
		int temp = test_pod[i];
		RUN_TEST(temp == resize_default_val, "resize test 2", result);
	}

	test_pod.resize(test_arr_size - 2, resize_default_val);

	for (uint i = 0; i < test_arr_size - 2; ++i)
	{
		RUN_TEST(test_pod[i] == test_arr[i], "Resize test 3", result);
	}

	/* Copy constructor */
	pod_vector<int> test_pod_copy = test_pod;

	RUN_TEST(test_pod.get_size() == test_pod_copy.get_size(), "Copy test 1", result);

	for (uint i = 0; i < test_pod.get_size(); ++i)
	{
		RUN_TEST(test_pod[i] == test_pod_copy[i], "Copy test 2", result);
	}

	/* Assignment */
	pod_vector<int> test_pod_copy_2;
	test_pod_copy_2 = test_pod;

	RUN_TEST(test_pod.get_size() == test_pod_copy_2.get_size(), "Copy test 3", result);

	for (uint i = 0; i < test_pod.get_size(); ++i)
	{
		RUN_TEST(test_pod[i] == test_pod_copy_2[i], "Copy test 4", result);
	}

	return result;
}

bool test_pod_vector_iterator()
{
	bool result = true;

	int base_arr[4] = { 123, -6123, 5, 4124124 };
	pod_vector<int> test_pod(4);

	for (uint i = 0; i < 4; ++i)
	{
		test_pod.push_back(base_arr[i]);
	}

	{
		int i = 0;
		for (pod_vector<int>::Iterator it = test_pod.get_begin(); it != test_pod.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "Iterator test 1", result);
		}
	}
	{
		int i = 0;
		for (pod_vector<int>::ConstIterator it = test_pod.get_begin(); it != test_pod.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "ConstIterator test 1", result);
		}
	}

	{
		pod_vector<int>::Iterator it = test_pod.get_begin() + 1;
		test_pod.insert_it(it, 69);
		RUN_TEST(test_pod.get_size() == 5, "Iterator insert test 1", result);
		RUN_TEST(test_pod[0] == base_arr[0], "Iterator insert test 2", result);
		RUN_TEST(test_pod[1] == 69, "Iterator insert test 3", result);
		RUN_TEST(test_pod[2] == base_arr[1], "Iterator insert test 4", result);
	}

	{
		pod_vector<int>::Iterator remove_it = test_pod.get_begin() + 1;
		test_pod.remove_it(remove_it);

		RUN_TEST(test_pod.get_size() == 4, "Iterator remove test 1", result);
		int i = 0;
		for (pod_vector<int>::ConstIterator it = test_pod.get_begin(); it != test_pod.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "Iterator remove test 2", result);
		}
	}

	{
		pod_vector<int>::Iterator swap_it = test_pod.get_begin() + 1;
		test_pod.swap_out_it(swap_it);
		RUN_TEST(test_pod.get_size() == 3, "Iterator swap out test 1", result);
		RUN_TEST(test_pod[1] == base_arr[3], "Iterator swap out test 2", result);
	}

	return result;
}

/* NON POD */
bool test_vector()
{
	bool result = true;

	/* Should trigger static assert */
	//vector<int> should_assert;

	{
		/* Testing non_pod destruction */
		non_pod temp(3);
		temp.get_data();
	}

	const uint start_capacity = 8;
	vector<non_pod> test_vec(start_capacity);

	RUN_TEST(test_vec.empty(), "Empty test 1", result);
	RUN_TEST(test_vec.get_capacity() == start_capacity, "Capacity test 1", result);
	RUN_TEST(test_vec.get_size() == 0, "Size test 1", result);

	/* Should assert */
	//test_vec.pop_back();

	/* push_back tests */
	test_vec.push_back(non_pod(1));
	test_vec.push_back(non_pod(2));

	RUN_TEST(!test_vec.empty(), "Empty test 2", result);
	RUN_TEST(test_vec.get_size() == 2, "Size test 2", result);

	/* Indexing tests */
	non_pod first = test_vec[0];
	RUN_TEST(first == non_pod(1), "Get at index test 1", result);
	const non_pod second = test_vec[1];
	RUN_TEST(second == non_pod(2), "Get at index test 2", result);

	/* Should assert */
	//test_vec[4];

	/* Automatic capacity increase */
	for (int i = 3; i < start_capacity + 2; ++i)
		test_vec.push_back(i);

	RUN_TEST(test_vec.get_capacity() > start_capacity, "Capacity test 2", result);
	RUN_TEST(test_vec.get_size() == start_capacity + 1, "Size test 3", result);
	RUN_TEST(test_vec[test_vec.get_size() - 1] == start_capacity + 1, "Get at index test 3", result);

	/* pop_back testing */
	const uint size_after_push = test_vec.get_size();
	test_vec.pop_back();
	RUN_TEST(test_vec.get_size() == size_after_push - 1, "Size test 4", result);

	/* swap_out testing */
	test_vec.swap_out(0);
	RUN_TEST(test_vec[0] == start_capacity, "Get at index test 4", result);
	RUN_TEST(test_vec.get_size() == size_after_push - 2, "Size test 5", result);

	/* trim testing */
	test_vec.trim();
	RUN_TEST(test_vec.get_size() == test_vec.get_capacity(), "Trim test 1", result);

	/* remove testing */
	non_pod third = test_vec[2];
	test_vec.remove(1);
	RUN_TEST(test_vec[1] == third, "Erase test 1", result);
	uint size_temp = test_vec.get_size();
	test_vec.remove(size_temp - 1);
	RUN_TEST(test_vec.get_size() == size_temp - 1, "Erase test 2", result);

	/* insert testing */
	uint size_before_insert = test_vec.get_size();
	first = test_vec[0];
	test_vec.insert(0, non_pod(16));
	RUN_TEST(test_vec[0] == non_pod(16), "Insert test 1", result);
	RUN_TEST(test_vec[1] == first, "Insert test 2", result);
	RUN_TEST(test_vec.get_size() == size_before_insert + 1, "Insert test 3", result);
	non_pod last = test_vec[test_vec.get_size() - 1];
	test_vec.insert(test_vec.get_size() - 1, non_pod(-123));
	RUN_TEST(test_vec[test_vec.get_size() - 2] == non_pod(-123), "Insert test 4", result);
	RUN_TEST(test_vec[test_vec.get_size() - 1] == last, "Insert test 5", result);

	/* reserve testing */
	uint current_capacity = test_vec.get_capacity();
	test_vec.reserve(current_capacity * 2);
	RUN_TEST(test_vec.get_capacity() == current_capacity * 2, "Reserve test 1", result);
	RUN_TEST(test_vec[0] == non_pod(16), "Reserve test 2", result);
	RUN_TEST(test_vec[1] == first, "Reserve test 3", result);
	RUN_TEST(test_vec[test_vec.get_size() - 2] == non_pod(-123), "Reserve test 4", result);
	RUN_TEST(test_vec[test_vec.get_size() - 1] == last, "Reserve test 5", result);

	/* clear */
	test_vec.clear();

	RUN_TEST(test_vec.empty(), "Empty test 3", result);
	RUN_TEST(test_vec.get_size() == 0, "Size test 6", result);

	/* Should fail */
	//RUN_TEST(test_vec.get_size() != 0, "Test test 1", result);

	/* resize */
	const uint test_arr_size = 5;
	const int resize_default_val = 0;
	non_pod test_arr[test_arr_size] = { non_pod(45), non_pod(-123), non_pod(63), non_pod(-6132), non_pod(12) };
	for (uint i = 0; i < test_arr_size; ++i)
		test_vec.push_back(test_arr[i]);

	test_vec.resize(test_arr_size + 2, resize_default_val);
	for (uint i = 0; i < test_arr_size; ++i)
	{
		RUN_TEST(test_vec[i] == test_arr[i], "Resize test 1", result);
	}
	for (uint i = test_arr_size; i < test_arr_size + 2; ++i)
	{
		non_pod temp = test_vec[i];
		RUN_TEST(temp == resize_default_val, "resize test 2", result);
	}

	test_vec.resize(test_arr_size - 2, resize_default_val);

	for (uint i = 0; i < test_arr_size - 2; ++i)
	{
		RUN_TEST(test_vec[i] == test_arr[i], "Resize test 3", result);
	}

	/* Copy constructor */
	vector<non_pod> test_vec_copy = test_vec;

	RUN_TEST(test_vec.get_size() == test_vec_copy.get_size(), "Copy test 1", result);

	for (uint i = 0; i < test_vec.get_size(); ++i)
	{
		RUN_TEST(test_vec[i] == test_vec_copy[i], "Copy test 2", result);
	}

	/* Assignment */
	vector<non_pod> test_vec_copy_2;
	test_vec_copy_2 = test_vec;

	RUN_TEST(test_vec.get_size() == test_vec_copy_2.get_size(), "Copy test 3", result);

	for (uint i = 0; i < test_vec.get_size(); ++i)
	{
		RUN_TEST(test_vec[i] == test_vec_copy_2[i], "Copy test 4", result);
	}

	return result;
}

bool test_vector_iterator()
{
	bool result = true;

	non_pod base_arr[4] = { non_pod(123), non_pod(-6123), non_pod(5), non_pod(4124124) };
	vector<non_pod> test_vec(4);

	for (uint i = 0; i < 4; ++i)
	{
		test_vec.push_back(base_arr[i]);
	}

	{
		int i = 0;
		for (vector<non_pod>::Iterator it = test_vec.get_begin(); it != test_vec.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "Iterator test 1", result);
		}
	}
	{
		int i = 0;
		for (vector<non_pod>::ConstIterator it = test_vec.get_begin(); it != test_vec.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "ConstIterator test 1", result);
		}
	}
	
	{
		vector<non_pod>::Iterator it = test_vec.get_begin() + 1;
		test_vec.insert_it(it, 69);
		RUN_TEST(test_vec.get_size() == 5, "Iterator insert test 1", result);
		RUN_TEST(test_vec[0] == base_arr[0], "Iterator insert test 2", result);
		RUN_TEST(test_vec[1] == 69, "Iterator insert test 3", result);
		RUN_TEST(test_vec[2] == base_arr[1], "Iterator insert test 4", result);
	}
	
	{
		vector<non_pod>::Iterator remove_it = test_vec.get_begin() + 1;
		test_vec.remove_it(remove_it);
	
		RUN_TEST(test_vec.get_size() == 4, "Iterator remove test 1", result);
		int i = 0;
		for (vector<non_pod>::ConstIterator it = test_vec.get_begin(); it != test_vec.get_end(); ++it, ++i)
		{
			RUN_TEST(*it == base_arr[i], "Iterator remove test 2", result);
		}
	}
	
	{
		vector<non_pod>::Iterator swap_it = test_vec.get_begin() + 1;
		test_vec.swap_out_it(swap_it);
		RUN_TEST(test_vec.get_size() == 3, "Iterator swap out test 1", result);
		RUN_TEST(test_vec[1] == base_arr[3], "Iterator swap out test 2", result);
	}

	return result;
}

DEA_END()