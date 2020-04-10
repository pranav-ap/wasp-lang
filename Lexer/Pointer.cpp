#include "pch.h"
#include "Pointer.h"

LEXER_API void Pointer::advance()
{
	index++;
}

LEXER_API void Pointer::retreat()
{
	index--;
}

LEXER_API int Pointer::get_index() const
{
	return index;
}