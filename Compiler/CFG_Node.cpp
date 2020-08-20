#pragma once
#include "pch.h"
#include "CFG_Node.h"

void Instruction_Node::push(Instruction instruction)
{
	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}