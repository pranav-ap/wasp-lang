#pragma once
#include "Pointer.h"

void Pointer::advance()
{
	index++;
}

void Pointer::retreat()
{
	index--;
}

int Pointer::get_index() const
{
	return index;
}