#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "Bytecode.h"
#include "Statement.h"
#include "BytecodeGenerator.h"
#include <memory>

class COMPILER_API Compiler
{
	BytecodeGenerator_ptr bytecode_generator;
public:
	Compiler()
		: bytecode_generator(std::make_shared<BytecodeGenerator>()) {};

	Bytecode_ptr execute(const Module_ptr module_ast);
};

using Compiler_ptr = COMPILER_API std::shared_ptr<Compiler>;
