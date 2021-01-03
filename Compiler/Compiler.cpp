#pragma once
#include "pch.h"
#include "Compiler.h"
#include "CFGBuilder.h"
#include "CFGAssembler.h"
#include "InstructionPrinter.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::byte;
using std::wstring;
using std::map;
using std::make_shared;
using std::make_optional;
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;
using std::begin;
using std::end;

std::tuple<ObjectStore_ptr, CodeObject_ptr> Compiler::run(const Module_ptr ast)
{
	// Compile

	set_current_scope(ast->scope);

	emit(OpCode::START);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	emit(OpCode::STOP);

	// Create CFG

	CFGBuilder_ptr cfg_builder = std::make_unique<CFGBuilder>(constant_pool, current_scope->code_object);
	CFG_ptr cfg = cfg_builder->create();
	// cfg->print(name_map);

	// Assemble CFG

	CFGAssembler_ptr cfg_assembler = std::make_unique<CFGAssembler>();
	CodeObject_ptr assembled_code_object = cfg_assembler->assemble(cfg);

	// Print

	InstructionPrinter_ptr printer = std::make_unique<InstructionPrinter>(constant_pool, name_map);
	printer->print(assembled_code_object);

	return std::make_tuple(constant_pool, assembled_code_object);
}
