#pragma once
#include "pch.h"
#include "BasicBlock.h"

void BasicBlock::push(ByteVector instruction)
{
	code_object->instructions.insert(
		std::end(code_object->instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}