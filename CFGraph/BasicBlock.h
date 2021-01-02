#pragma once
#include "CodeObject.h"
#include "OpCode.h"
#include <memory>

#ifdef CFGRAPH_EXPORTS
#define CFGRAPH_API __declspec(dllexport)
#else
#define CFGRAPH_API __declspec(dllimport)
#endif

CFGRAPH_API enum class BlockType
{
	Unknown,
	JumpOnFalse,
	ConnectToFollowingBlock,
	JustJump,
	Start,
	Stop
};

struct CFGRAPH_API BasicBlock
{
	BlockType type;
	CodeObject_ptr code_object;

	BasicBlock()
		: type(BlockType::Unknown),
		code_object(std::make_shared<CodeObject>()) {};

	BasicBlock(BlockType type)
		: type(type),
		code_object(std::make_shared<CodeObject>()) {};
};

using BasicBlock_ptr = CFGRAPH_API std::shared_ptr<BasicBlock>;
