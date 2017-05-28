#ifndef DEA_CORE_VECTOR_H
#define DEA_CORE_VECTOR_H

#include "core/container/byte_array.h"

#include <new> // TODO: Memory manager

DEA_START()

/* TODO: Add static constructor option */

template <typename T>
class vector : public byte_array
{
public:
	typedef T* Iterator;
	typedef const T* ConstIterator;

public:
	vector(const uint capacity = 4);
	vector(const vector<T> &other);
	~vector();

	vector<T> &operator=(const vector<T>& other);
	vector<T> &operator=(const vector<T>&& other);

	inline T &operator[](const uint index);
	inline const T &operator[](const uint index) const;

	inline T *get_pointer();
	inline const T *get_pointer() const;

	inline Iterator get_begin();
	inline ConstIterator get_begin() const;
	inline Iterator get_end();
	inline ConstIterator get_end() const;

	void push_back(const T &item);
	void pop_back();	
	void insert(uint index, const T &item);
	void insert_it(ConstIterator it, const T &item);
	void remove(uint index);
	void remove_it(ConstIterator it);
	void swap_out(uint index);
	void swap_out_it(ConstIterator it);

	void reserve(uint new_capacity);
	void resize(uint new_size, const T &data);
	void trim();
	void clear();

protected:
	vector(T *begin, const uint size, const uint capacity) = delete; // TODO: Implement
};

/* Do I really want this as it will spam quite a lot of new classes? */
/* TODO: Heap version */
/* TODO: Heap static version */

#include "core/container/vector.inl"

DEA_END()

#endif // DEA_CORE_VECTOR_H
