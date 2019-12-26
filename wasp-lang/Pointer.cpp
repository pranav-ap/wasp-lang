#include "Pointer.h"

void Pointer::advance()
{
	this->index++;
}

void Pointer::retreat()
{
	this->index--;
}

int Pointer::get_index()
{
	return this->index;
}