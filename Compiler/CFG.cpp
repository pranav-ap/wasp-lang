#pragma once
#include "pch.h"
#include "CFG.h"
#include "CFG_Node.h"
#include "Assertion.h"
#include <variant>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void CFG::add_node(CFG_Node_ptr node)
{
	if (start_node == nullptr)
	{
		start_node = node;
		current_node = start_node;
	}

	std::visit(overloaded{
		[&](Basic_Block& block)
		{
			block.outgoing_link = node;
		},
		[&](Terminal_Node& terminal_node)
		{
			terminal_node.outgoing_link = node;
		},
		[](auto) {}
		}, *current_node);

	current_node = node;
}

void CFG::add_node(CFG_Node_ptr node, bool true_path)
{
	if (start_node == nullptr)
	{
		start_node = node;
		current_node = start_node;
	}

	ASSERT(std::holds_alternative<Condition_Node>(*current_node), "Does not hold a Conditional_Basic_Block");
	auto block = std::get_if<Condition_Node>(&*current_node);

	if (true_path)
	{
		block->true_outgoing_link = node;
	}
	else
	{
		block->false_outgoing_link = node;
	}

	current_node = node;
}

void CFG::add_local_cfg(CFG_Node_ptr local_start_node, CFG_Node_ptr local_end_node)
{
	add_node(local_start_node);
	current_node = local_end_node;
}

void CFG::add_local_cfg(CFG_Node_ptr local_start_node, CFG_Node_ptr local_end_node, bool true_path)
{
	add_node(local_start_node, true_path);
	current_node = local_end_node;
}

// Emit

void CFG::emit(Instruction instruction)
{
	std::visit(overloaded{
		[&](Condition_Node& block)
		{
			block.push(instruction);
		},
		[&](Basic_Block& block)
		{
			block.push(instruction);
		},
		[](auto) {}
		}, *current_node);
}