#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <utility>

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

// Emit

void Compiler::emit(OpCode opcode)
{
	NULL_CHECK(current_scope);
	current_scope->code_object->emit(opcode);
}

void Compiler::emit(OpCode opcode, int operand)
{
	NULL_CHECK(current_scope);
	current_scope->code_object->emit(opcode, operand);
}

void Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	NULL_CHECK(current_scope);
	current_scope->code_object->emit(opcode, operand_1, operand_2);
}

// Scope

void Compiler::set_current_scope(SymbolScope_ptr new_scope)
{
	current_scope = new_scope;
}

ByteVector Compiler::leave_scope()
{
	NULL_CHECK(current_scope);
	ByteVector instructions = current_scope->code_object->instructions;

	OPT_CHECK(current_scope->enclosing_scope);
	current_scope = current_scope->enclosing_scope.value();

	current_scope->code_object->instructions.insert(
		end(current_scope->code_object->instructions),
		begin(instructions),
		end(instructions)
	);

	return instructions;
}

ByteVector Compiler::leave_subroutine_scope()
{
	NULL_CHECK(current_scope);
	ByteVector instructions = current_scope->code_object->instructions;

	OPT_CHECK(current_scope->enclosing_scope);
	current_scope = current_scope->enclosing_scope.value();

	return instructions;
}

// Utils

int Compiler::create_label()
{
	int label = next_label++;
	return label;
}

wstring Compiler::concat(StringVector items, wstring connector)
{
	wstring final_string = L"";

	for (const auto member : items)
	{
		final_string.append(connector);
		final_string.append(member);
	}

	final_string = final_string.substr(2, final_string.size());
	return final_string;
}

wstring Compiler::extract_identifier_from_type_pattern(Expression_ptr expression)
{
	ASSERT(holds_alternative<TypePattern>(*expression), "Expected a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*expression);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Expected an Identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	return identifier->name;
}