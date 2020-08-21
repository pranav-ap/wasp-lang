#pragma once
#include "pch.h"
#include "CFGBuilder.h"
#include "Compiler.h"

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	cfg_builder = std::make_shared<CFGBuilder>();
	auto cfg = cfg_builder->execute(module_ast);

	Bytecode_ptr bytecode;

	return bytecode;
}