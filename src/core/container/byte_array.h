#ifndef DEA_CORE_BYTE_ARRAY_H
#define DEA_CORE_BYTE_ARRAY_H

DEA_START()

class byte_array
{
	static const uintptr POINTER_MASK = sizeof(char *) == 4 ? 0xfffffffc : 0xfffffffffffffffc;
	static const uintptr USER_RESERVED = 0x1;
	static const uintptr STATIC_BUFFER = 0x2;
	static const uintptr INFO_MASK = USER_RESERVED | STATIC_BUFFER;

protected:
	static const uint SMALLEST_AUTOMATIC_SIZE = 4;

public:
	byte_array();
	byte_array(uintptr buffer, uint size, uint capacity, bool user_reserved, bool static_buffer);
	byte_array(const byte_array &other) = delete;
	~byte_array();

	byte_array &operator=(const byte_array& other) = delete;
	byte_array &operator=(const byte_array&& other) = delete;

	inline bool empty() const { return size == 0; }
	inline uint get_size() const { return size; }
	inline uint get_capacity() const { return capacity; }

protected:
	inline bool is_static() const { return (packed_ptr & STATIC_BUFFER) != 0; }
	inline bool is_user_reserved() const { return (packed_ptr & USER_RESERVED) != 0; }

	inline char *get_byte_pointer() { return reinterpret_cast<char *>(packed_ptr & POINTER_MASK); }
	inline const char *get_byte_pointer() const { return reinterpret_cast<char *>(packed_ptr & POINTER_MASK); }
	inline char *get_byte_pointer_indexed(const uint index, const uint type_size)
	{ 
		dea_assert(index < size && "Invalid index");
		return get_byte_pointer() + (index * type_size); 
	}
	inline const char *get_byte_pointer_indexed(uint index, uint type_size) const
	{ 
		dea_assert(index < size && "Invalid index");
		return get_byte_pointer() + (index * type_size); 
	}

	uintptr create_buffer(uint capacity, uint type_size) const;
	void move_data(uint object_count, uint type_size, uintptr source, uintptr destination) const;
	void set_buffer(uintptr buffer, uint object_count, uint buffer_capacity, bool user_reserved, bool static_buffer);
	void destroy_buffer(uintptr buffer);
	void create_and_replace_buffer(uint new_capacity, uint type_size);

	void byte_move(const byte_array &other, uint type_size);
	void byte_copy(const byte_array &other, uint type_size);

	void byte_push_back(uint type_size, const char *data);
	void byte_pop_back();
	void byte_swap_out(uint index, uint type_size);
	void byte_remove(uint index, uint type_size);
	void byte_insert(uint index, uint type_size, const char *data);
	void byte_reserve(uint new_capacity, uint type_size);
	void byte_resize(uint new_size, uint type_size, const char *data);
	void byte_trim(uint type_size);
	void byte_clear();

	inline int calculate_next_pow2(int val)
	{
		if (val < 0)
			return 0;
		--val;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		return val + 1;
	}

	inline uint get_new_allocation_size()
	{
		uint new_capacity = (uint)calculate_next_pow2((int)capacity + 1);
		/* The first few allocations are expensive so just skip those */
		if (new_capacity < SMALLEST_AUTOMATIC_SIZE)
			new_capacity = SMALLEST_AUTOMATIC_SIZE;

		return new_capacity;
	}

protected:
	/* Pointer to the buffer with the first two bits reserved for flags */
	uintptr packed_ptr;
	uint size;
	uint capacity;

private:

};

DEA_END()

#endif // DEA_CORE_BYTE_ARRAY_H