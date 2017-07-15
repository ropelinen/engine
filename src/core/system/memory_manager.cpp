#include "core/config/precompiled.h"
#include "core/system/memory_manager.h"

#include <cstring>

DEA_START()

#if DEA_BUILD_TYPE == DEA_DEBUG
#define OVERWRITE_TEST 1
#endif
#define ALLOCATED_BYTE 0xA1
#define FREE_BYTE 0xFE

static const uint32 FREE_LIST_BLOCK_COUNT = 10000;
static const uint32 INVALID_OFFSET = 0xFFFFFFFF;
	
struct debug_data
{
	uint32 free_memory; // TODO: Implement
	uint32 used_memory; // TODO: Implement
	uint32 current_allocations; // TODO: Implement
	uint32 total_allocations; // TODO: Implement
	uint32 total_deallocations; // TODO:Implement
};

struct free_block_nfo
{
	uint32 offset;
	uint32 size;
};

/* The first free list will be at the start of the memory */
struct free_list
{
	uint32 next_list_offset;
	uint32 used_blocks;
	free_block_nfo data_start;
};

/* TODO: Insert all the debug data collecting. */
memory_manager::memory_manager(void *memory, uint32 size, void *debug_memory)
{
	dea_assert(memory && "Trying to create memory manager with NULL memory block");
	dea_assert(size && "Trying to create memory manager with zero size");

	this->memory = (uintptr)memory;
	memory_amount = size;

	free_list *list = (free_list *)memory;
	list->next_list_offset = INVALID_OFFSET;
	list->used_blocks = 1;
	uint32 free_list_size = sizeof(free_list) + sizeof(free_block_nfo) * (FREE_LIST_BLOCK_COUNT - 1);
	dea_assert(free_list_size < size && "Free list is larger than size of memory, can't create so small managers");
	
	list->data_start.offset = free_list_size;
	list->data_start.size = size - free_list_size;

#ifdef OVERWRITE_TEST
	void *mem = (void *)(this->memory + free_list_size);
	memset(mem, FREE_BYTE, size - free_list_size);
#endif
	
#if DEA_MEMORY_DEBUGGING == DEA_FALSE
	(debug_data *)debug_memory;
	this->debug_memory = NULL;
#else
	debug_data *debug_mem = (debug_data *)debug_memory;
	this->debug_memory = debug_mem;
	if (debug_mem)
	{
		debug_mem->free_memory = size;
		debug_mem->used_memory = 0;
		debug_mem->current_allocations = 0;
		debug_mem->total_allocations = 0;
		debug_mem->total_deallocations = 0;
	}
#endif
}

memory_manager::~memory_manager()
{
	/* Don't do anything, we don't own the memory block. */
}

void *memory_manager::get_memory(uint32 size)
{
	uint32 free_list_offset = 0;
	uint32 total_size = size + sizeof(uint32);
	
	while (free_list_offset != INVALID_OFFSET)
	{
		free_list *list = (free_list *)(memory + free_list_offset);
		free_list_offset = list->next_list_offset;

		free_block_nfo *blocks = &list->data_start;
		for (uint32 i = 0; i < list->used_blocks; ++i)
		{
			if (blocks[i].size > total_size)
			{
				uint32 *mem = (uint32 *)(memory + blocks[i].offset);
				*mem = total_size;
				++mem;
				
				blocks[i].size -= total_size;
				blocks[i].offset += total_size;

#ifdef OVERWRITE_TEST
				uint8 *mem_bytes = (uint8 *)mem;
				for (uint byte = 0; byte < size; ++byte)
					dea_assert(mem_bytes[byte] == FREE_BYTE && "Allocating already allocated memory");

				memset(mem_bytes, ALLOCATED_BYTE, size);
#endif
				return mem;
			}
			else if (blocks[i].size == total_size)
			{
				uint32 *mem = (uint32 *)(memory + blocks[i].offset);
				*mem = total_size;
				++mem;
				
				if (free_list_offset == INVALID_OFFSET)
				{
					list->used_blocks -= 1;
					blocks[i] = blocks[list->used_blocks];
				}
				else
				{
					free_list *last_list = list;
					while (last_list->used_blocks == FREE_LIST_BLOCK_COUNT && last_list->next_list_offset != INVALID_OFFSET)
					{
						free_list *next_list = (free_list *)(memory + last_list->next_list_offset);
						if (next_list->used_blocks == 0)
							break;
						
						last_list = next_list;
					}

					dea_assert(last_list->used_blocks && "No used blocks, something went wrong");
					last_list->used_blocks -= 1;
					free_block_nfo *last_data = &last_list->data_start;
					blocks[i] = last_data[last_list->used_blocks];
				}
#ifdef OVERWRITE_TEST
				uint8 *mem_bytes = (uint8 *)mem;
				for (uint byte = 0; byte < size; ++byte)
					dea_assert(mem_bytes[byte] == FREE_BYTE && "Allocating already allocated memory");
				
				memset(mem_bytes, ALLOCATED_BYTE, size);
#endif
				return mem;
			}
		}
	}

	dea_assert(0 && "Manager out of memory");
	return NULL;
}

void *memory_manager::get_zeroed_memory(uint32 size)
{
	void *mem_block = get_memory(size);
	memset(mem_block, 0, size);
	return mem_block;
}

void memory_manager::free_memory(void *ptr)
{
	dea_assert((uintptr)ptr - sizeof(uint32) >= memory
			   && (uintptr)ptr < memory + memory_amount
			   && "Trying to free memory that is not from this manager");

	uintptr fixed_ptr = (uintptr)ptr - sizeof(uint32);
	uint32 size = *((uint32 *)fixed_ptr);

#ifdef OVERWRITE_TEST
	memset((void *)fixed_ptr, FREE_BYTE, size);
#endif
	
	free_list *list = (free_list *)memory;
	while (list->used_blocks == FREE_LIST_BLOCK_COUNT && list->next_list_offset != INVALID_OFFSET)
		list = (free_list *)(memory + list->next_list_offset);
	
	if (list->used_blocks < FREE_LIST_BLOCK_COUNT)
	{
		/* Free to the front of the list to prevent fragmentation */
		free_block_nfo *data = &list->data_start;
		data[list->used_blocks].offset = data[0].offset;
		data[list->used_blocks].size = data[0].size;
		data[0].offset = (uint32)(fixed_ptr - memory);
		data[0].size = size;
		list->used_blocks += 1;
	}
	else
	{
		/* Allocate new free list */
		uint32 free_list_size = sizeof(free_list) + sizeof(free_block_nfo) * (FREE_LIST_BLOCK_COUNT - 1);
		free_list *new_list = (free_list *)get_memory(free_list_size);

		if (!new_list)
		{
			dea_assert(0 && "Couldn't allocate new free list, freeing memory failed");
			return;
		}

		list->next_list_offset = (uint32)((uintptr)new_list - memory);
		new_list->next_list_offset = INVALID_OFFSET;
		if (list->used_blocks < FREE_LIST_BLOCK_COUNT)
		{
			new_list->used_blocks = 0;
			free_block_nfo *data = &list->data_start;
			data[list->used_blocks].offset = (uint32)(fixed_ptr - memory);
			data[list->used_blocks].size = size;
			list->used_blocks += 1;
		}
		else
		{
			/* Free to the end of the old list to prevent
			 * fragmentation */
			free_block_nfo *data = &list->data_start;
			new_list->data_start.offset = data[FREE_LIST_BLOCK_COUNT - 1].offset;
			new_list->data_start.size = data[FREE_LIST_BLOCK_COUNT - 1].size;
			new_list->used_blocks = 1;
			data[FREE_LIST_BLOCK_COUNT - 1].offset = (uint32)(fixed_ptr - memory);
			data[FREE_LIST_BLOCK_COUNT - 1].size = size;
		}
	}
}

void memory_manager::defragment()
{
	/* TODO: Implement */
}

uint32 memory_manager::get_debug_data_size() const
{
#if DEA_MEMORY_DEBUGGING == DEA_TRUE
	return sizeof(debug_data);
#else
	return 0;
#endif
}

#undef OVERWRITE_TEST

DEA_END()
