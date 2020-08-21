#pragma once
#include "Bytecode.h"

#include <vector>
#include <memory>
#include <optional>

enum class CFGNodeType
{
	Condition,
	Basic_Block,
	Terminal
};

struct CFGNode;
using CFGNode_ptr = std::shared_ptr<CFGNode>;

struct CFGNode
{
	CFGNodeType type;

	Instructions instructions;

	std::optional<CFGNode_ptr> true_outgoing_link;
	std::optional<CFGNode_ptr> false_outgoing_link;

	std::optional<CFGNode_ptr> outgoing_link;

	void push(Instruction instruction);

	CFGNode() : type(CFGNodeType::Terminal) {};
	CFGNode(CFGNodeType type) : type(type) {};
};