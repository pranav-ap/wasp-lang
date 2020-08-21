#pragma once
#include "pch.h"
#include "CFG.h"
#include "CFG_Node.h"
#include "Assertion.h"

void CFG::add_node(CFGNode_ptr node)
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

void CFG::extend(CFGNode_ptr local_start_node, CFGNode_ptr local_end_node)
{
	add_node(local_start_node);
	current_node = local_end_node;
}