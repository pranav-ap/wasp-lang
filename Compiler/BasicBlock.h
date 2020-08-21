#pragma once
#include "Bytecode.h"
#include <vector>
#include <memory>
#include <optional>

enum class BasicBlockType
{
	Conditional,
	Unconditional
};

struct BasicBlock;
using BasicBlock_ptr = std::shared_ptr<BasicBlock>;

struct BasicBlock
{
	BasicBlockType type;

	Instructions instructions;

	std::optional<BasicBlock_ptr> true_outgoing_link;
	std::optional<BasicBlock_ptr> false_outgoing_link;

	std::optional<BasicBlock_ptr> outgoing_link;

	void push(Instruction instruction);

	BasicBlock() : type(BasicBlockType::Unconditional) {};
	BasicBlock(BasicBlockType type) : type(type) {};
};