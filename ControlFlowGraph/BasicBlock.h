#pragma once

#ifdef CONTROLFLOWGRAPH_EXPORTS
#define CONTROLFLOWGRAPH_API __declspec(dllexport)
#else
#define CONTROLFLOWGRAPH_API __declspec(dllimport)
#endif

#include "MemorySystem.h"
#include "OpCode.h"
#include <memory>

enum class CONTROLFLOWGRAPH_API BlockType
{
	Unknown,
	ConditionalJump,
	Unconditional,
	UnconditionalJump,
	Stop
};

struct CONTROLFLOWGRAPH_API BasicBlock
{
	int label;

	BlockType type;
	CodeSection_ptr code_section;

	BasicBlock()
		: label(-1),
		type(BlockType::Unknown),
		code_section(std::make_shared<CodeSection>()) {};

	void push(ByteVector instruction);
};

using BasicBlock_ptr = CONTROLFLOWGRAPH_API std::shared_ptr<BasicBlock>;
