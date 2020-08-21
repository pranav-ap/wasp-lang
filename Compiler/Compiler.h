#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "Bytecode.h"
#include "Statement.h"
#include "CFG.h"
#include "CFGBuilder.h"
#include <memory>

class COMPILER_API Compiler
{
	CFGBuilder_ptr cfg_builder;
public:
	Compiler() {};
	Bytecode_ptr execute(const Module_ptr module_ast);
};

using Compiler_ptr = COMPILER_API std::shared_ptr<Compiler>;
