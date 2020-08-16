#pragma once

#ifdef BYTECODE_EXPORTS
#define BYTECODE_API __declspec(dllexport)
#else
#define BYTECODE_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "ObjectSystem.h"

#include <memory>
#include <vector>

struct BYTECODE_API Bytecode
{
	Instructions instructions;
	std::vector<Object_ptr> constant_pool;
	std::vector<int> relative_jumps;

	Bytecode(Instructions instructions, std::vector<Object_ptr> constant_pool, std::vector<int> relative_jumps)
		: instructions(instructions),
		constant_pool(constant_pool),
		relative_jumps(relative_jumps) {};
};

using Bytecode_ptr = BYTECODE_API std::shared_ptr<Bytecode>;
