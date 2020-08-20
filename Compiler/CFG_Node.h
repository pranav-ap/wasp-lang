#pragma once
#include "Bytecode.h"

#include <vector>
#include <memory>
#include <optional>
#include <variant>

struct Condition_Node;
struct Basic_Block;
struct Terminal_Node;

using CFG_Node = std::variant<
	std::monostate,
	Condition_Node,
	Basic_Block,
	Terminal_Node
>;

using CFG_Node_ptr = std::shared_ptr<CFG_Node>;

struct Instruction_Node
{
	Instructions instructions;
	void push(Instruction instruction);
};

struct Condition_Node : public Instruction_Node
{
	std::optional<CFG_Node_ptr> true_outgoing_link;
	std::optional<CFG_Node_ptr> false_outgoing_link;
};

struct Basic_Block : public Instruction_Node
{
	std::optional<CFG_Node_ptr> outgoing_link;
};

struct Terminal_Node
{
	std::optional<CFG_Node_ptr> outgoing_link;
};

using Condition_Node_ptr = std::shared_ptr<Condition_Node>;
using Basic_Block_ptr = std::shared_ptr<Basic_Block>;
using Terminal_Node_ptr = std::shared_ptr<Terminal_Node>;
