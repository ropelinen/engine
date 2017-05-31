#include "core/config/precompiled.h"
#include "core/container/string.h"

#include <cstring> // TODO: Make own versions of strlen and strcpy

DEA_START()

string::string()
	: byte_array()
{
	uintptr buffer = create_buffer(1, sizeof(char));
	set_buffer(buffer, 1, 1, false, false);
	get_byte_pointer()[0] = '\0';
}

string::string(const char *str)
	: byte_array()
{
	dea_assert(str && "Trying to construct string from a NULL ptr");
	uint buf_len = (uint)strlen(str) + 1;
	uintptr buffer = create_buffer(buf_len, sizeof(char));
	set_buffer(buffer, buf_len, buf_len, false, false);
	strcpy_s(get_byte_pointer(), buf_len, str);
}

string::string(const string &other)
	: byte_array()
{
	byte_copy(other, sizeof(char));
}

string::string(string &&other)
{
	byte_move(other, sizeof(char));
}

string::~string()
{
}

DEA_END()
