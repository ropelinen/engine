#include "core/config/precompiled.h"
#include "core/container/byte_array.h"

#include <cstring>
#include <stdlib.h> /* TODO: Memory manager */


DEA_START()

byte_array::byte_array()
	: packed_ptr(0)
	, size(0)
	, capacity(0)
{
}

byte_array::byte_array(uintptr buffer, uint size, uint capacity, bool user_reserved, bool static_buffer)
	: size(size)
	, capacity(capacity)
{
	set_buffer(buffer, size, capacity, user_reserved, static_buffer);
}

byte_array::~byte_array()
{
	if (packed_ptr)
		destroy_buffer(packed_ptr);
}

uintptr byte_array::create_buffer(uint buffer_capacity, uint type_size) const
{
	/* TODO: Memory manager */
	uintptr ptr = reinterpret_cast<uintptr>(malloc(buffer_capacity * type_size));
	dea_assert((INFO_MASK & ptr) == 0 && "Buffer pointer uses mask bits");
	return ptr;
}

void byte_array::move_data(uint object_count, uint type_size, uintptr source, uintptr destination) const
{
	if (source == destination)
		return;

	const size_t bytes = object_count * type_size;
	char *dst = (char *)(destination & POINTER_MASK);
	char *src = (char *)(source & POINTER_MASK);

	dea_assert(src != dst && "Trying to move from same location but with different info bits");
	dea_assert((dst > src ? src + bytes < dst : dst + bytes < src) && "Trying to move data with overlapping memory");

	memcpy(dst, src, bytes);
}

void byte_array::set_buffer(uintptr buffer, uint object_count, uint buffer_capacity, bool user_reserved, bool static_buffer)
{
	dea_assert((INFO_MASK & buffer) == 0 && "Buffer pointer uses mask bits");

	uintptr mask_bits = 0;
	if (user_reserved)
		mask_bits |= USER_RESERVED;
	if (static_buffer)
		mask_bits |= STATIC_BUFFER;

	packed_ptr = buffer | mask_bits;

	size = object_count;
	capacity = buffer_capacity;
}

void byte_array::destroy_buffer(uintptr buffer)
{
	dea_assert(buffer && "Trying to destroy a null buffer");

	if (!is_user_reserved())
	{
		char *buf = reinterpret_cast<char *>(buffer & POINTER_MASK);
		/* TODO: Memory manager */
		free(buf);
	}
}

void byte_array::create_and_replace_buffer(uint new_capacity, uint type_size)
{
	dea_assert(!is_static() && "Trying to replace a static buffer");

	uintptr new_buffer = create_buffer(new_capacity, type_size);
	uintptr old_buffer = packed_ptr;

	if (new_capacity < size)
		size = new_capacity;

	move_data(size, type_size, old_buffer, new_buffer);
	if (!is_user_reserved() && old_buffer)
		destroy_buffer(old_buffer);

	set_buffer(new_buffer, size, new_capacity, false, false);
}

void byte_array::byte_move(const byte_array &other, uint type_size)
{
	dea_assert(!is_static() && !other.is_static() && "Trying to move to/from static array");

	if (!is_user_reserved())
		destroy_buffer(packed_ptr);
	if (!other.is_user_reserved())
	{
		packed_ptr = other.packed_ptr;
		size = other.size;
		capacity = other.capacity;
	}
	else
	{
		byte_copy(other, type_size);
	}
}

void byte_array::byte_copy(const byte_array &other, uint type_size)
{
	if (capacity < other.capacity)
	{
		dea_assert(!is_static() && "Trying to copy to a static array from a larger array");
		create_and_replace_buffer(other.capacity, type_size);
	}

	move_data(other.size, type_size, other.packed_ptr, packed_ptr);
	size = other.size;
	capacity = other.capacity;
}

void byte_array::byte_push_back(uint type_size, const char *data)
{
	if (size < capacity)
	{
		char *buf = get_byte_pointer() + (size * type_size);
		memcpy(buf, data, type_size);
		++size;
	}
	else
	{
		if (is_static())
		{
			dea_assert(0 && "Trying to resize a static buffer");
		}
		else
		{
			create_and_replace_buffer(get_new_allocation_size(), type_size);

			char *buf = get_byte_pointer() + (size * type_size);
			memcpy(buf, data, type_size);
			++size;
		}
	}
}

void byte_array::byte_pop_back()
{
	dea_assert(!empty() && "Trying to pop from an empty container");
	--size;
}

void byte_array::byte_swap_out(uint index, uint type_size)
{
	dea_assert(!empty() && "Trying to swap out from an empty container");
	dea_assert(index < size && "Invalid index");

	/* If it happens to be the last element we only have to reduce the size */
	if (index < size - 1)
	{
		char *at_index = get_byte_pointer_indexed(index, type_size);
		char *last = get_byte_pointer_indexed(size - 1, type_size);
		memcpy(at_index, last, type_size);
	}

	--size;
}

void byte_array::byte_remove(uint index, uint type_size)
{
	dea_assert(size && "Trying to remove from an empty container");
	dea_assert(index < size && "Invalid index");

	/* If it happens to be the last element we only have to reduce the size */
	if (index < size - 1)
	{
		char *at_index = get_byte_pointer_indexed(index, type_size);
		char *move_start = at_index + type_size;
		uint bytes_to_move = (uint)(get_byte_pointer_indexed(size - 1, type_size) - at_index);
		memmove(at_index, move_start, bytes_to_move);
	}

	--size;
}

void byte_array::byte_insert(uint index, uint type_size, const char *data)
{
	dea_assert(index < size && "Invalid index");

	if (size >= capacity)
	{
		if (is_static())
		{
			dea_assert(0 && "Trying to resize a static buffer");
		}
		else
		{
			create_and_replace_buffer(get_new_allocation_size(), type_size);
		}
	}

	char *at_index = get_byte_pointer_indexed(index, type_size);
	char *move_target = at_index + type_size;
	uint bytes_to_move = (uint)(get_byte_pointer_indexed(size - 1, type_size) + type_size - at_index);
	memmove(move_target, at_index, bytes_to_move);
	memcpy(at_index, data, type_size);
	++size;
}

void byte_array::byte_reserve(uint new_capacity, uint type_size)
{
	if (capacity >= new_capacity)
		return;

	create_and_replace_buffer(new_capacity, type_size);
}

void byte_array::byte_resize(uint new_size, uint type_size, const char *data)
{
	uint old_size = size;
	create_and_replace_buffer(new_size, type_size);

	capacity = new_size;
	size = new_size;

	char *ptr = get_byte_pointer();
	for (uint i = old_size; i < size; ++i)
	{
		memcpy(ptr + (i * type_size), data, type_size);
	}
}

void byte_array::byte_trim(uint type_size)
{
	dea_assert(!is_static() && "Trying to trim a static vector");

	if (size > 0)
	{
		create_and_replace_buffer(size, type_size);
	}
	else
	{
		if (!is_user_reserved())
		{
			destroy_buffer(packed_ptr);
			packed_ptr = 0;
		}
	}
}

void byte_array::byte_clear()
{
	size = 0;
}

DEA_END()