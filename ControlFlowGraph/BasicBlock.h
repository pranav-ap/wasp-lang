#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "Bytecode.h"
#include <memory>

enum class CONTROLFLOWGRAPH_API BlockType
{
	Unknown,
	Conditional,
	Unconditional
};

struct BasicBlock;
using BasicBlock_ptr = CONTROLFLOWGRAPH_API std::shared_ptr<BasicBlock>;

struct CONTROLFLOWGRAPH_API BasicBlock
{
	int id;
	int label;

	BlockType type;
	Instructions instructions;

	BasicBlock() : id(-1), label(-1), type(BlockType::Unknown) {};
	BasicBlock(int id, BlockType type) : id(id), label(-1), type(type) {};

	void push(Instruction instruction);
};
