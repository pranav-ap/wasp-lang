#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif


#include "MemorySystem.h"
#include "OpCode.h"
#include <memory>

enum class COMPILER_API BlockType
{
	Unknown,
	ConditionalJump,
	Unconditional,
	UnconditionalJump,
	Stop
};

struct COMPILER_API BasicBlock
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

using BasicBlock_ptr = COMPILER_API std::shared_ptr<BasicBlock>;
