#pragma once
#include "Bytecode.h"
#include <memory>

enum class BlockType
{
	Unknown,
	ConditionalBasicBlock,
	UnconditionalBasicBlock
};

struct BasicBlock;
using BasicBlock_ptr = std::shared_ptr<BasicBlock>;

struct BasicBlock
{
	int label;
	int outgoing_id;

	BlockType type;
	Instructions instructions;

	BasicBlock() : label(0), type(BlockType::Unknown) {};
	BasicBlock(int label, BlockType type) : label(label), outgoing_id(-1), type(type) {};

	void push(Instruction instruction);
};
