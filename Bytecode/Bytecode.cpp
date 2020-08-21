#include "pch.h"
#include "Bytecode.h"
#include <iostream>

#define GET_OPCODE std::to_integer<int>(instructions.at(index))
#define INC_INDEX index++

void Bytecode::print()
{
}