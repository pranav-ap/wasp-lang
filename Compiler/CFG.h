#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif


#include "OpCode.h"
#include "BasicBlock.h"
#include "MemorySystem.h"
#include <memory>
#include <string>
#include <map>

class COMPILER_API CFG
{
	ConstantPool_ptr constant_pool;

public:
	int start_node_id;

	std::map<int, std::pair<int, int>> adjacency_list; // id -> (id, id)
	std::map<int, BasicBlock_ptr> node_id_to_basic_blocks;

	CFG()
		: start_node_id(0),
		constant_pool(std::make_shared<ConstantPool>()) {};

	CFG(ConstantPool_ptr constant_pool)
		: start_node_id(0),
		constant_pool(constant_pool) {};

	void print();
};

using CFG_ptr = COMPILER_API std::shared_ptr<CFG>;
