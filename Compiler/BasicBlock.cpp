#pragma once
#include "pch.h"
#include "BasicBlock.h"

void BasicBlock::push(Instruction instruction)
{
	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}