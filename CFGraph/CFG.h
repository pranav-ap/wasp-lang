#pragma once
#include "OpCode.h"
#include "BasicBlock.h"
#include "ObjectStore.h"
#include <memory>
#include <string>
#include <map>

#ifdef CFGRAPH_EXPORTS
#define CFGRAPH_API __declspec(dllexport)
#else
#define CFGRAPH_API __declspec(dllimport)
#endif

class CFGRAPH_API CFG
{
	ObjectStore_ptr object_store;

public:
	int start_block_id;
	int end_block_id;

	std::map<int, BasicBlock_ptr> basic_blocks;

	std::map<int, std::pair<int, int>> adjacency_list; // block id -> (block id, block id)
	std::map<int, int> label_to_block_id; // label -> block id

	CFG()
		: start_block_id(0),
		end_block_id(0),
		object_store(std::make_shared<ObjectStore>()) {};

	CFG(ObjectStore_ptr object_store)
		: start_block_id(0),
		end_block_id(0),
		object_store(object_store) {};

	void print();
};

using CFG_ptr = CFGRAPH_API std::shared_ptr<CFG>;
