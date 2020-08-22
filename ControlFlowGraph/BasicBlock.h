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
	BlockType type;
	Instructions instructions;

	BasicBlock() : type(BlockType::Unknown) {};
	BasicBlock(BlockType type) : type(type) {};

	void push(Instruction instruction);
};
