#include "pch.h"
#include "Pointer.h"

UTILS_API void Pointer::advance()
{
	this->index++;
}

UTILS_API void Pointer::retreat()
{
	this->index--;
}

UTILS_API int Pointer::get_index() const
{
	return this->index;
}