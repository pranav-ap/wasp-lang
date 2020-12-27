#pragma once

#include "CodeObject.h"
#include "OpCode.h"
#include <memory>

enum class BlockType
{
	Unknown,
	ConditionalJump,
	Unconditional,
	UnconditionalJump,
	Start,
	Stop
};

struct BasicBlock
{
	int label;
	BlockType type;
	CodeObject_ptr code_object;

	BasicBlock()
		: label(-1),
		type(BlockType::Unknown),
		code_object(std::make_shared<CodeObject>()) {};

	void push(ByteVector instruction);
};

using BasicBlock_ptr = std::shared_ptr<BasicBlock>;
