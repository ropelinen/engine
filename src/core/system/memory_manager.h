#ifndef DEA_CORE_MEMORY_MANAGER_H
#define DEA_CORE_MEMORY_MANAGER_H

DEA_START()

class memory_manager
{
public:
	memory_manager(void *memory, uint32 size, void *debug_memory = NULL);
	~memory_manager();

	void *get_memory(uint32 size);
	void *get_zeroed_memory(uint32 size);
	void free_memory(void *ptr);
	void defragment();
	
	uint32 get_debug_data_size() const;
	
private:
	uintptr memory;
	void *debug_memory;
	uint32 memory_amount;
};

DEA_END()

#endif // DEA_CORE_MEMORY_MANAGER_H
