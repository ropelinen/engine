#include "core/config/precompiled.h"
#include "core/system/memory_manager.h"

#include <cstring>

DEA_START()

/* TODO: Insert all the debug data collecting. */

memory_manager::memory_manager(void *memory, uint32 size, void *debug_memory)
{
	dea_assert(memory && "Trying to create memory manager with NULL memory block");
	dea_assert(size && "Trying to create memory manager with zero size");

	this->memory = (uintptr)memory;
	memory_amount = size;
	first_free_offset = 0;
	header *hdr = (header *)memory;
	hdr->size = size;
	hdr->next_free_offset = INVALID_OFFSET;

#if DEA_MEMORY_DEBUGGING == DEA_FALSE
	(debug_data *)debug_memory;
	this->debug_memory = NULL;
#else
	this->debug_memory = (debug_data *)debug_memory;
	if (this->debug_memory)
	{
		this->debug_memory->free_memory = size;
		this->debug_memory->used_memory = 0;
		this->debug_memory->current_allocations = 0;
		this->debug_memory->total_allocations = 0;
		this->debug_memory->total_deallocations = 0;
	}
#endif
}

memory_manager::~memory_manager()
{
	/* Don't do anything, we don't own the memory block. */
}

void *memory_manager::get_memory(uint32 size)
{
	dea_assert(first_free_offset != INVALID_OFFSET && "No memory left");

	void *mem = NULL;
	header *hdr = (header *)(memory + first_free_offset);
	uint32 *prev_offset_ptr = &first_free_offset;
	
	while (true)
	{
		uint32 block_size = hdr->size;
		if (block_size < size)
		{
			if (hdr->next_free_offset != INVALID_OFFSET)
			{
				dea_assert(hdr->next_free_offset < memory_amount && "Invalid offset");
			    prev_offset_ptr = &hdr->next_free_offset;
				hdr = (header *)(memory + hdr->next_free_offset);
			}
			else
			{
				dea_assert(0 && "Memory too fragmented, couldn't find big enough free chunk");
				break;
			}
		}
		else if (block_size >= size + sizeof(header))
		{
			hdr->size = size;
			mem = (void *)(&hdr->data);
			header *new_header = (header *)(memory + *prev_offset_ptr + sizeof(uint32) + size);
			new_header->size = block_size - size - sizeof(uint32);
			new_header->next_free_offset = hdr->next_free_offset;
			*prev_offset_ptr = (uint32)((uintptr)new_header - memory);
			break;
		}
		else
		{
			*prev_offset_ptr = hdr->next_free_offset;
			mem = (void *)(&hdr->data);
			break;
		}
	}

	return mem;
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

	header *hdr = (header *)((uintptr)ptr - sizeof(uint32));
	uint32 new_offset = (uint32)((uintptr)hdr - memory);
	dea_assert(new_offset < memory_amount && "Invalid offset");
	
	if (first_free_offset != INVALID_OFFSET)
	{
		dea_assert(first_free_offset < memory_amount && "Invalid offset");
		
		if (new_offset > first_free_offset)
		{
			uint32 prev_offset = first_free_offset;
			uint32 next_offset = first_free_offset;
			
			while (true)
			{
				header *current_header = (header *)(memory + next_offset);
				prev_offset = next_offset;
				next_offset = current_header->next_free_offset;

				if (next_offset == INVALID_OFFSET)
				{
					current_header->next_free_offset = new_offset;
					hdr->next_free_offset = INVALID_OFFSET;
					return;
				}
				else if (new_offset < next_offset)
				{
					dea_assert(next_offset < memory_amount && "Invalid offset");
					hdr->next_free_offset = current_header->next_free_offset;
					current_header->next_free_offset = new_offset;
					break;
				}
			}
		}
		else
		{
			hdr->next_free_offset = first_free_offset;
			first_free_offset = new_offset;
			return;
		}
	}
	else
	{
		first_free_offset = new_offset;
		return;
	}
}

void memory_manager::defragment()
{
	/* TODO: Implement */
}

DEA_END()
