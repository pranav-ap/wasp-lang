#pragma once
#include "pch.h"
#include "BytecodeGenerator.h"
#include "Compiler.h"

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	Bytecode_ptr bytecode = bytecode_generator->execute(module_ast);

	return bytecode;
}