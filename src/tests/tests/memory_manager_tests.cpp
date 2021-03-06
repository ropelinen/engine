#include "tests/config/precompiled.h"
#include "tests/tests/memory_manager_tests.h"

#include "core/system/memory_manager.h"

#include <cstdlib>
#include <Windows.h>

DEA_START()

#define ALLOC_COUNT 15000

struct allocation
{
	void *mem;
	size_t size;
};

void get_duration_micro(LARGE_INTEGER freq, LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER &duration_micro)
{
	duration_micro.QuadPart = end.QuadPart - start.QuadPart;
	duration_micro.QuadPart *= 1000000;
	duration_micro.QuadPart /= freq.QuadPart;
}

bool test_memory_manager()
{
	bool result = true;

	allocation allocs[ALLOC_COUNT];
	
	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER elapsed_microseconds_malloc;
	QueryPerformanceCounter(&start_time);
	for (uint i = 0; i < ALLOC_COUNT; ++i)
	{
		allocs[i].mem = malloc(512);
	}
	QueryPerformanceCounter(&end_time);

	get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_malloc);

	LARGE_INTEGER elapsed_microseconds_free;
	QueryPerformanceCounter(&start_time);
	for (uint i = 0; i < ALLOC_COUNT; ++i)
	{
		free(allocs[i].mem);
	}
	QueryPerformanceCounter(&end_time);

	get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_free);

	uint32 manager_mem_size = ALLOC_COUNT * 512;
	manager_mem_size += manager_mem_size / 2;
	void *manager_mem_block = malloc(manager_mem_size);

	{
		memory_manager manager(manager_mem_block, manager_mem_size);

		LARGE_INTEGER elapsed_microseconds_get_mem;
		QueryPerformanceCounter(&start_time);
		for (uint i = 0; i < ALLOC_COUNT; ++i)
		{
			allocs[i].mem = manager.get_memory(512);
		}
		QueryPerformanceCounter(&end_time);

		get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_get_mem);

		LARGE_INTEGER elapsed_microseconds_free_mem;
		QueryPerformanceCounter(&start_time);
		for (uint i = 0; i < ALLOC_COUNT; ++i)
		{
			manager.free_memory(allocs[i].mem);
		}
		QueryPerformanceCounter(&end_time);

		get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_free_mem);

		RUN_TEST(elapsed_microseconds_malloc.QuadPart >= elapsed_microseconds_get_mem.QuadPart, "Allocate speed test 1", result);
		printf("--malloc: %lld, get_mem: %lld\n", elapsed_microseconds_malloc.QuadPart, elapsed_microseconds_get_mem.QuadPart);

		RUN_TEST(elapsed_microseconds_free.QuadPart >= elapsed_microseconds_free_mem.QuadPart, "Free speed test 1", result);
		printf("--free: %lld, free_mem: %lld\n", elapsed_microseconds_free.QuadPart, elapsed_microseconds_free_mem.QuadPart);
	}

	{
		memory_manager manager(manager_mem_block, manager_mem_size);

		LARGE_INTEGER elapsed_microseconds_default;
		QueryPerformanceCounter(&start_time);
		for (uint loop = 0; loop < 5; ++loop)
		{
			for (uint i = 0; i < ALLOC_COUNT; ++i)
				allocs[i].mem = malloc(512);

			for (uint i = 0; i < ALLOC_COUNT; ++i)
				free(allocs[i].mem);
		}
		QueryPerformanceCounter(&end_time);
		get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_default);

		LARGE_INTEGER elapsed_microseconds_manager;
		QueryPerformanceCounter(&start_time);
		for (uint loop = 0; loop < 5; ++loop)
		{
			for (uint i = 0; i < ALLOC_COUNT; ++i)
				allocs[i].mem = manager.get_memory(512);

			for (uint i = 0; i < ALLOC_COUNT; ++i)
				manager.free_memory(allocs[i].mem);
		}
		QueryPerformanceCounter(&end_time);
		get_duration_micro(frequency, start_time, end_time, elapsed_microseconds_manager);

		RUN_TEST(elapsed_microseconds_manager.QuadPart < elapsed_microseconds_default.QuadPart, "Total speed test 1", result);
		printf("--default: %lld, manager: %lld\n", elapsed_microseconds_default.QuadPart, elapsed_microseconds_manager.QuadPart);
    }
	
	free(manager_mem_block);

	return result;
}

DEA_END()
