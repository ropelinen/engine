#ifndef DEA_CORE_MEMORY_MANAGER_H
#define DEA_CORE_MEMORY_MANAGER_H

DEA_START()

class memory_manager
{
	static const uint32 INVALID_OFFSET = 0xFFFFFFFF;
	
	struct debug_data
	{
		uint32 free_memory;
		uint32 used_memory;
		uint32 current_allocations;
		uint32 total_allocations;
		uint32 total_deallocations;
	};

	/* This is located in memory so that the size is before the
	 * returned pointer and the union part is the start of the
	 * actually used memory */
	struct header
	{
		uint32 size;
		union
		{
			uint32 next_free_offset;
			uint32 data; // Just have this as uint32 as a pointer
						 // could be 64bit
		};
	};
	
public:
	memory_manager(void *memory, uint32 size, void *debug_memory = NULL);
	~memory_manager();

	void *get_memory(uint32 size);
	void *get_zeroed_memory(uint32 size);
	void free_memory(void *ptr);
	void defragment();
	
	inline uint32 get_debug_data_size() const { return sizeof(debug_data); }
	
private:
	uintptr memory;
	uint32 memory_amount;
	uint32 first_free_offset;
	debug_data *debug_memory;
};

DEA_END()

#endif // DEA_CORE_MEMORY_MANAGER_H
