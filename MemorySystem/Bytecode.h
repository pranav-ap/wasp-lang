#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include "OpCode.h"

#include <memory>
#include <vector>
#include <map>
#include <string>

class MEMORYSYSTEM_API Bytecode
{
public:
	std::vector<std::byte> instructions;

	void push(std::vector<std::byte> instruction);
	void replace(int index, std::byte replacement);
	int length();

	// static utils

	static int get_opcode_arity(std::byte opcode);
	static int get_opcode_arity(OpCode opcode);
};

using Bytecode_ptr = MEMORYSYSTEM_API std::shared_ptr<Bytecode>;
