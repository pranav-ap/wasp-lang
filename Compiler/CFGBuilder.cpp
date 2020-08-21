#pragma once
#include "pch.h"
#include "CFG.h"
#include "BasicBlock.h"
#include "Assertion.h"
#include "CFGBuilder.h"

void CFGBuilder::add_node(BasicBlock_ptr node)
{
	if (start_node == nullptr)
	{
		start_node = node;
		current_node = start_node;
	}
	else
	{
		current_node->outgoing_link = node;
		current_node = node;
	}
}

void CFGBuilder::extend(BasicBlock_ptr local_start_node, BasicBlock_ptr local_end_node)
{
	add_node(local_start_node);
	current_node = local_end_node;
}

BasicBlock_ptr CFGBuilder::execute(Bytecode_ptr bytecode)
{
	return BasicBlock_ptr();
}