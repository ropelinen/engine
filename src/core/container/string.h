#ifndef DEA_CORE_STRING_H
#define DEA_CORE_STRING_H

#include "core/container/byte_array.h"

DEA_START()

class string : public byte_array
{
public:
	string();
	string(const char *str);
	string(const string &other);
	string(string &&other);
	~string();

	inline string &operator=(const string &other) { byte_copy(other, sizeof(char)); return *this; }
	inline string &operator=(string &&other) { byte_move(other, sizeof(char)); return *this; }

	inline const char *c_str() { return get_byte_pointer(); }
};

DEA_END()

#endif // DEA_CORE_STRING_H
