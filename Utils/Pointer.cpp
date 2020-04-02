#include "pch.h"
#include "Pointer.h"

UTILS_API void Pointer::advance()
{
	index++;
}

UTILS_API void Pointer::retreat()
{
	index--;
}

UTILS_API int Pointer::get_index() const
{
	return index;
}