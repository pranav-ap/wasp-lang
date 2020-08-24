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
	Unconditional,
	UnconditionalJump,
	Start,
	Stop
};

struct CONTROLFLOWGRAPH_API BasicBlock
{
	int label;

	BlockType type;
	ByteVector instructions;

	BasicBlock() : label(-1), type(BlockType::Unknown) {};

	void push(ByteVector instruction);
};

using BasicBlock_ptr = CONTROLFLOWGRAPH_API std::shared_ptr<BasicBlock>;
