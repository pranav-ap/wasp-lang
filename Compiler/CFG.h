#pragma once
#include "OpCode.h"
#include "BasicBlock.h"
#include "ObjectStore.h"
#include <memory>
#include <string>
#include <map>

class CFG
{
	ObjectStore_ptr object_store;

public:
	int start_node_id;
	std::map<int, std::pair<int, int>> adjacency_list; // id -> (id, id)
	std::map<int, BasicBlock_ptr> basic_blocks;

	CFG()
		: start_node_id(0),
		object_store(std::make_shared<ObjectStore>()) {};

	CFG(ObjectStore_ptr object_store)
		: start_node_id(0),
		object_store(object_store) {};

	void print();
};

using CFG_ptr = std::shared_ptr<CFG>;
