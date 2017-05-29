template <typename T>
vector<T>::vector(const uint capacity = 4, bool is_static = false)
{
	/* If this triggers you should use pod_vector */
	dea_is_not_pod_assert(T);

	uintptr buffer = create_buffer(capacity, sizeof(T));
	set_buffer(buffer, 0, capacity, false, is_static);
}

template <typename T>
vector<T>::vector(const vector<T> &other)
{
	/* If this triggers you should use pod_vector */
	dea_is_not_pod_assert(T);

	uintptr buffer = create_buffer(other.get_capacity(), sizeof(T));
	set_buffer(buffer, other.get_size(), capacity, false, false);

	Iterator this_it = get_begin();
	ConstIterator other_it = other.get_begin();
	for (; other_it != other.get_end(); ++this_it, ++other_it)
	{
		new (reinterpret_cast<char*>(this_it)) T(*other_it);
	}
}

template <typename T>
vector<T>::~vector() { clear(); }

template <typename T>
vector<T> &vector<T>::operator=(const vector<T>& other)
{
	dea_assert(this != &other && "Self assing");
	dea_assert(!is_static() && "Trying to assing to a static vector");

	if (size > 0)
	{
		for (Iterator it = get_begin(); it != get_end(); ++it)
		{
			it->~T();
		}
	}

	if (!is_user_reserved())
		destroy_buffer(packed_ptr);

	uintptr buffer = create_buffer(other.get_capacity(), sizeof(T));
	set_buffer(buffer, other.get_size(), capacity, false, false);

	Iterator this_it = get_begin();
	ConstIterator other_it = other.get_begin();
	for (; other_it != other.get_end(); ++this_it, ++other_it)
	{
		new (reinterpret_cast<char*>(this_it)) T(*other_it);
	}

	return *this;
}

template <typename T>
vector<T> &vector<T>::operator=(const vector<T>&& other)
{
	dea_assert(this != &other && "Self move");
	dea_assert(!is_static() && "Trying to assing to a static vector");

	if (size > 0)
	{
		for (Iterator it = get_begin(); it != get_end(); ++i)
		{
			it->~T();
		}
	}

	if (!is_user_reserved())
		destroy_buffer(packed_ptr);

	packed_ptr = other.packed_ptr;
	size = other.size;
	capacity = other.capacity;

	other.packed_ptr = 0;
	other.size = 0;
	other.capacity = 0;

	return *this
}

template <typename T>
T &vector<T>::operator[](const uint index) 
{ 
	return reinterpret_cast<T &>(*get_byte_pointer_indexed(index, sizeof(T))); 
}

template <typename T>
const T &vector<T>::operator[](const uint index) const 
{ 
	return reinterpret_cast<T &>(*get_byte_pointer_indexed(index, sizeof(T))); 
}

template <typename T>
T *vector<T>::get_pointer() 
{ 
	return reinterpret_cast<T *>(get_byte_pointer()); 
}

template <typename T>
const T *vector<T>::get_pointer() const 
{ 
	return reinterpret_cast<T *>(get_byte_pointer()); 
}

template <typename T>
typename vector<T>::Iterator vector<T>::get_begin()
{ 
	return reinterpret_cast<Iterator>(get_byte_pointer()); 
}

template <typename T>
typename vector<T>::ConstIterator vector<T>::get_begin() const 
{ 
	 return reinterpret_cast<ConstIterator>(get_byte_pointer()); 
}

template <typename T>
typename vector<T>::Iterator vector<T>::get_end() 
{ 
	return (reinterpret_cast<Iterator>(get_byte_pointer())) + size; 
}

template <typename T>
typename vector<T>::ConstIterator vector<T>::get_end() const 
{ 
	return (reinterpret_cast<ConstIterator>(get_byte_pointer())) + size; 
}

template <typename T>
void vector<T>::push_back(const T &item)
{
	if (size >= capacity)
	{
		if (is_static())
		{
			dea_assert(0 && "Trying to resize a static buffer");
		}
		else
		{
			uint new_capacity = get_new_allocation_size();
			uintptr old_buffer = packed_ptr;
			uintptr new_buffer = create_buffer(new_capacity, sizeof(T));
			Iterator old_it = get_begin();
			Iterator old_end = get_end();
			set_buffer(new_buffer, size, new_capacity, false, false);

			for (Iterator it = get_begin(); old_it != old_end; ++it, ++old_it)
			{
				new (reinterpret_cast<char*>(it)) T(*old_it);
				old_it->~T();
			}

			destroy_buffer(old_buffer);
		}
	}

	T *buf = get_pointer();
	new (reinterpret_cast<char*>(buf + size)) T(item);
	++size;
}

template <typename T>
void vector<T>::pop_back()
{
	dea_assert(!empty() && "Trying to pop from empty container");
	T *buf = get_pointer();
	buf[size - 1].~T();
	--size;
}

template <typename T>
void vector<T>::insert(uint index, const T &item)
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
			uint new_capacity = get_new_allocation_size();
			uintptr old_buffer = packed_ptr;
			uintptr new_buffer = create_buffer(new_capacity, sizeof(T));
			Iterator old_it = get_begin();
			Iterator old_end = get_end();
			set_buffer(new_buffer, size, new_capacity, false, false);

			for (Iterator it = get_begin(); old_it != old_end; ++it, ++old_it)
			{
				new (reinterpret_cast<char*>(it)) T(*old_it);
				old_it->~T();
			}

			destroy_buffer(old_buffer);
		}
	}

	T *ptr = get_pointer();
	for (int i = (int)size - 1; i >= (int)index; --i)
	{
		new (reinterpret_cast<char*>(ptr + i + 1)) T(*(ptr + i));
		(ptr + i)->~T();
	}

	new (reinterpret_cast<char*>(ptr + index)) T(item);
	++size;
}

/* Could be done way better */
template <typename T>
void vector<T>::insert_it(ConstIterator it, const T &item)
{
	dea_assert(it >= get_begin() && it < get_end() && "Invalid iterator");

	T *ptr = reinterpret_cast<T *>(get_byte_pointer());
	insert((uint)(it - ptr), item);
}

template <typename T>
void vector<T>::remove(uint index)
{
	dea_assert(size && "Trying to remove from an empty container");
	dea_assert(index < size && "Invalid index");

	/* If it happens to be the last element we only have to destroy it */
	T *last = get_pointer() + size - 1;
	if (index < size - 1)
	{
		T *at_index = get_pointer() + index;
		T *move_start = at_index + 1;
		for (; move_start != get_end(); ++at_index, ++move_start)
		{
			at_index->~T();
			new (reinterpret_cast<char*>(at_index)) T(*move_start);
			move_start->~T();
		}
	}
	else
		last->~T();

	--size;
}

/* Could be done way better */
template <typename T>
void vector<T>::remove_it(ConstIterator it)
{
	dea_assert(it >= get_begin() && it < get_end() && "Invalid iterator");

	T *ptr = reinterpret_cast<T *>(get_byte_pointer());
	remove((uint)(it - ptr));
}

template <typename T>
void vector<T>::swap_out(uint index)
{
	dea_assert(!empty() && "Trying to swap out from an empty container");
	dea_assert(index < size && "Invalid index");

	/* If it happens to be the last element we only have to destroy it */
	T *last = get_pointer() + size - 1;
	if (index < size - 1)
	{
		T *at_index = get_pointer() + index;
		at_index->~T();
		new (reinterpret_cast<char*>(at_index)) T(*last);
	}

	last->~T();
	--size;
}

/* Could be done way better */
template <typename T>
void vector<T>::swap_out_it(ConstIterator it)
{
	dea_assert(it >= get_begin() && it < get_end() && "Invalid iterator");

	T *ptr = reinterpret_cast<T *>(get_byte_pointer());
	swap_out((uint)(it - ptr));
}

template <typename T>
void vector<T>::reserve(uint new_capacity)
{
	dea_assert(!is_static() && "Trying to reserve on a static buffer");

	if (capacity >= new_capacity)
		return;

	bool old_user_reserved = is_user_reserved();
	uintptr old_buffer = packed_ptr;
	uintptr new_buffer = create_buffer(new_capacity, sizeof(T));
	Iterator old_it = get_begin();
	Iterator old_end = get_end();
	set_buffer(new_buffer, size, new_capacity, false, false);

	for (Iterator it = get_begin(); old_it != old_end; ++it, ++old_it)
	{
		new (reinterpret_cast<char*>(it)) T(*old_it);
		old_it->~T();
	}

	if (!old_user_reserved)
		destroy_buffer(old_buffer);
}

template <typename T>
void vector<T>::resize(uint new_size, const T &data)
{
	dea_assert(!is_static() && "Trying to resize a static vector");

	if (size == new_size)
		return;

	bool old_user_reserved = is_user_reserved();
	uint old_size = size;
	uintptr old_buffer = packed_ptr;
	uintptr new_buffer = create_buffer(new_size, sizeof(T));
	Iterator old_it = get_begin();
	Iterator old_end = get_end();
	set_buffer(new_buffer, new_size, new_size, false, false);

	Iterator it;
	for (it = get_begin(); old_it != old_end && it != get_end(); ++it, ++old_it)
	{
		new (reinterpret_cast<char*>(it)) T(*old_it);
		old_it->~T();
	}

	while (old_it != old_end)
	{
		old_it->~T();
		++old_it;
	}
	
	if (old_size < new_size)
	{
		for (; it != get_end(); ++it)
		{
			new (reinterpret_cast<char*>(it)) T(data);
		}
	}

	if (!old_user_reserved)
		destroy_buffer(old_buffer);
}

template <typename T>
void vector<T>::trim()
{
	dea_assert(!is_static() && "Trying to trim a static vector");

	if (size > 0)
	{
		bool old_user_reserved = is_user_reserved();
		uint new_capacity = size;
		uintptr old_buffer = packed_ptr;
		uintptr new_buffer = create_buffer(new_capacity, sizeof(T));
		Iterator old_it = get_begin();
		Iterator old_end = get_end();
		set_buffer(new_buffer, size, new_capacity, false, false);

		for (Iterator it = get_begin(); old_it != old_end; ++it, ++old_it)
		{
			new (reinterpret_cast<char*>(it)) T(*old_it);
			old_it->~T();
		}

		if (!old_user_reserved)
			destroy_buffer(old_buffer);
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

template <typename T>
void vector<T>::clear()
{
	for (Iterator it = get_begin(); it != get_end(); ++it)
	{
		it->~T();
	}
	size = 0;
}
