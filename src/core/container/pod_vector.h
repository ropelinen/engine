#ifndef DEA_CORE_POD_VECTOR_H
#define DEA_CORE_POD_VECTOR_H

#include "core/container/byte_array.h"

DEA_START()

/* TODO: Add static constructor option */

template <typename T>
class pod_vector : public byte_array
{
public:
	typedef T* Iterator;
	typedef const T* ConstIterator;

public:
	pod_vector(uint capacity = SMALLEST_AUTOMATIC_SIZE)
		: byte_array()
	{
		/* If this triggers you should use a normal vector */
		dea_is_pod_assert(T);

		uintptr buffer = create_buffer(capacity, sizeof(T));
		set_buffer(buffer, 0, capacity, false, false);
	}

	pod_vector(const pod_vector<T> &other)
	{
		/* If this triggers you should use a normal vector */
		dea_is_pod_assert(T);

		byte_copy(other, sizeof(T));
	}

	~pod_vector() { }

	inline pod_vector<T> &operator=(const pod_vector<T>& other) { byte_copy(other, sizeof(T)); return *this; }
	inline pod_vector<T> &operator=(const pod_vector<T>&& other) { byte_move(other, sizeof(T)); return *this; }

	inline T &operator[](uint index) { return reinterpret_cast<T &>(*get_byte_pointer_indexed(index, sizeof(T))); }
	inline const T &operator[](uint index) const { return reinterpret_cast<T &>(*get_byte_pointer_indexed(index, sizeof(T))); }

	inline T *get_pointer() { return reinterpret_cast<T *>(get_byte_pointer()); }
	inline const T *get_pointer() const { return reinterpret_cast<T *>(get_byte_pointer()); }

	inline Iterator get_begin() { return reinterpret_cast<Iterator>(get_byte_pointer()); }
	inline ConstIterator get_begin() const { return reinterpret_cast<ConstIterator>(get_byte_pointer()); }
	inline Iterator get_end() { return (reinterpret_cast<Iterator>(get_byte_pointer())) + size; }
	inline ConstIterator get_end() const { return (reinterpret_cast<ConstIterator>(get_byte_pointer())) + size; }

	void push_back(const T &item) { byte_push_back(sizeof(T), reinterpret_cast<const char *>(&item)); }
	void pop_back() { byte_pop_back(); }
	void insert(uint index, const T &item) { byte_insert(index, sizeof(T), reinterpret_cast<const char *>(&item));  }
	void insert_it(ConstIterator it, const T &item) { T *ptr = reinterpret_cast<T *>(get_byte_pointer()); byte_insert((uint)(it - ptr), sizeof(T), reinterpret_cast<const char *>(&item)); }
	void remove(uint index) { byte_remove(index, sizeof(T)); }
	void remove_it(ConstIterator it) { T *ptr = reinterpret_cast<T *>(get_byte_pointer()); byte_remove((uint)(it - ptr), sizeof(T)); }
	void swap_out(uint index) { byte_swap_out(index, sizeof(T)); }
	void swap_out_it(ConstIterator it) { T *ptr = reinterpret_cast<T *>(get_byte_pointer()); byte_swap_out((uint)(it - ptr), sizeof(T)); }

	void reserve(uint new_capacity) { byte_reserve(new_capacity, sizeof(T)); }
	void resize(uint new_size, const T &data) { byte_resize(new_size, sizeof(T), reinterpret_cast<const char *>(&data)); }
	void trim() { byte_trim(sizeof(T)); }
	void clear() { byte_clear(); }

protected:
	pod_vector(T *begin, const uint size, const uint capacity)
		: byte_array(begin, size, capacity, sizeof(T), true, false)
	{
		/* If this triggers you should use a normal vector */
		dea_is_pod_assert(T);
	}
};

/* TODO: Small version */
/* TODO: Small static version */

DEA_END()

#endif // DEA_CORE_POD_VECTOR_H