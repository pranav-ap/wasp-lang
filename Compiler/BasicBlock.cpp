#include "pch.h"
#include "BasicBlock.h"

void BasicBlock::push(ByteVector instruction)
{
	code_section->instructions.insert(
		std::end(code_section->instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}