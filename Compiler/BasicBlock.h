#pragma once
#include "Bytecode.h"

#include <vector>
#include <memory>
#include <optional>

struct BasicBlock;
using BasicBlock_ptr = std::shared_ptr<BasicBlock>;

struct BasicBlock
{
	Instructions instructions;

	std::optional<BasicBlock_ptr> true_outgoing_link;
	std::optional<BasicBlock_ptr> false_outgoing_link;

	std::optional<BasicBlock_ptr> absolute_outgoing_link;
};
