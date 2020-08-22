#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Bytecode.h"
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
using std::wstring;
using std::make_shared;
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	next_id = 0;
	next_label = 0;

	enter_scope();

	emit(OpCode::START);

	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	emit(OpCode::STOP);

	Instructions instructions = leave_scope();
	Bytecode_ptr bytecode = make_shared<Bytecode>(instructions, constant_pool, id_to_name);

	return bytecode;
}

// Statement

void Compiler::visit(const Statement_ptr statement)
{
	std::visit(overloaded{
		[&](Assignment const& stat) { visit(stat); },

		[&](Branching const& stat) { visit(stat); },
		[&](WhileLoop const& stat) { visit(stat); },
		[&](ForInLoop const& stat) { visit(stat); },
		[&](Pass const& stat) { visit(stat); },

		[&](VariableDefinition const& stat) { visit(stat); },
		[&](UDTDefinition const& stat) { visit(stat); },
		[&](AliasDefinition const& stat) { visit(stat); },
		[&](FunctionDefinition const& stat) { visit(stat); },
		[&](GeneratorDefinition const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },

		[&](ImportCustom const& stat) { visit(stat); },
		[&](ImportInBuilt const& stat) { visit(stat); },

		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](AssertStatement const& stat) { visit(stat); },

		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](YieldStatement const& stat) { visit(stat); },

		[](auto) { FATAL("Unexpected statement!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
}

void Compiler::visit(Assignment const& statement)
{
	visit(statement.expression);

	auto scope = scope_stack.top();
	int label = scope->symbol_table->lookup(statement.name);
	emit(OpCode::STORE_LOCAL, label);
}

void Compiler::visit(Branching const& statement)
{
	int exit_tree_label = create_label();
	int	branch_label = create_label();

	// Conditional branches

	int branch_index = 0;
	int last_branch_index = statement.branches.size() - 1;
	bool else_is_present = statement.else_block.size() > 0;

	for (const auto branch : statement.branches)
	{
		emit(OpCode::LABEL, branch_label);

		auto condition = branch.first;
		visit(condition);

		if (branch_index == last_branch_index && !else_is_present)
		{
			emit(OpCode::POP_JUMP_IF_FALSE, exit_tree_label);
		}
		else
		{
			branch_label = create_label();
			emit(OpCode::POP_JUMP_IF_FALSE, branch_label);
		}

		enter_scope();

		auto body = branch.second;
		visit(body);

		emit(OpCode::POP_JUMP, exit_tree_label);

		leave_scope();

		branch_index++;
	}

	// Else Branch

	if (statement.else_block.size() > 0)
	{
		enter_scope();
		emit(OpCode::LABEL, branch_label);
		visit(statement.else_block);
		leave_scope();
	}

	if (statement.else_block.size() == 0)
	{
		branch_label = exit_tree_label;
	}

	emit(OpCode::LABEL, exit_tree_label);
}

void Compiler::visit(WhileLoop const& statement)
{
	int condition_label = create_label();
	emit(OpCode::LABEL, condition_label);

	auto condition = statement.condition;
	visit(condition);

	auto scope = enter_scope();
	scope->continue_label = condition_label;

	int block_end_label = create_label();
	emit(OpCode::JUMP_IF_FALSE, block_end_label);
	scope->break_label = block_end_label;

	auto body = statement.block;
	visit(body);

	emit(OpCode::JUMP, condition_label);
	emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(ForInLoop const& statement)
{
	int iterate_label = create_label();
	emit(OpCode::LABEL, iterate_label);

	// Place iterable on stack

	auto iterable = statement.iterable;
	visit(statement.iterable);

	auto scope = enter_scope();

	int block_begin_label = create_label();
	emit(OpCode::LABEL, block_begin_label);
	scope->continue_label = block_begin_label;

	int block_end_label = create_label();
	scope->break_label = block_end_label;

	std::visit(overloaded{
		[&](wstring const& expr) { emit(OpCode::ITERATE_OVER_STRING, block_end_label); },
		[&](ListLiteral const& expr) { emit(OpCode::ITERATE_OVER_LIST, block_end_label); },
		[&](MapLiteral const& expr) { emit(OpCode::ITERATE_OVER_MAP, block_end_label); },
		[&](Identifier const& expr) { emit(OpCode::ITERATE_OVER_IDENTIFIER, block_end_label); },

		[](auto) { FATAL("Not an iterable!"); }
		}, *iterable);

	int item_id = define_variable(statement.item_name);
	emit(OpCode::STORE_LOCAL, item_id);

	auto body = statement.block;
	visit(body);

	emit(OpCode::JUMP, block_begin_label);
	emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(Pass const& statement)
{
	emit(OpCode::NO_OP);
}

void Compiler::visit(Break const& statement)
{
	auto scope = scope_stack.top();
	emit(OpCode::JUMP, scope->break_label);
}

void Compiler::visit(Continue const& statement)
{
	auto scope = scope_stack.top();
	emit(OpCode::JUMP, scope->continue_label);
}

void Compiler::visit(Return const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		emit(OpCode::RETURN_VALUE);
	}
	else
	{
		emit(OpCode::RETURN_VOID);
	}
}

void Compiler::visit(YieldStatement const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		emit(OpCode::YIELD_VALUE);
	}
	else
	{
		emit(OpCode::YIELD_VOID);
	}
}

void Compiler::visit(VariableDefinition const& statement)
{
	visit(statement.expression);

	int label = define_variable(statement.name);
	emit(OpCode::STORE_LOCAL, label);
}

void Compiler::visit(UDTDefinition const& statement)
{
}

void Compiler::visit(AliasDefinition const& statement)
{
}

void Compiler::visit(FunctionDefinition const& statement)
{
	enter_scope();

	for (const auto [arg_name, _] : statement.arguments)
	{
		define_variable(statement.name);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	int parameter_count = statement.arguments.size();

	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(instructions, parameter_count));

	int label = add_to_constant_pool(move(function_object));
	emit(OpCode::PUSH_CONSTANT, label);
}

void Compiler::visit(GeneratorDefinition const& statement)
{
}

void Compiler::visit(EnumDefinition const& statement)
{
}

void Compiler::visit(ImportCustom const& statement)
{
}

void Compiler::visit(ImportInBuilt const& statement)
{
}

void Compiler::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::POP_FROM_STACK);
}

void Compiler::visit(AssertStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::ASSERT);
}

// Expression

void Compiler::visit(const Expression_ptr expression)
{
	std::visit(overloaded{
		[&](double expr) { visit(expr); },
		[&](std::wstring expr) { visit(expr); },
		[&](bool expr) { visit(expr); },
		[&](ListLiteral const& expr) { visit(expr); },
		[&](TupleLiteral const& expr) { visit(expr); },
		[&](MapLiteral const& expr) { visit(expr); },
		[&](UDTConstruct const& expr) { visit(expr); },
		[&](UDTMemberAccess const& expr) { visit(expr); },
		[&](EnumMember const& expr) { visit(expr); },
		[&](Identifier const& expr) { visit(expr); },
		[&](Call const& expr) { visit(expr); },
		[&](Unary const& expr) { visit(expr); },
		[&](Binary const& expr) { visit(expr); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *expression);
}

void Compiler::visit(std::vector<Expression_ptr> const& expressions)
{
	for (const auto expr : expressions)
	{
		visit(expr);
	}
}

void Compiler::visit(const double number)
{
	int label = find_number_constant(number);

	if (label == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
		label = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, label);
}

void Compiler::visit(const std::wstring text)
{
	int label = find_string_constant(text);

	if (label == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(text));
		label = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, label);
}

void Compiler::visit(const bool boolean)
{
	if (boolean)
	{
		emit(OpCode::PUSH_CONSTANT_TRUE);
	}
	else
	{
		emit(OpCode::PUSH_CONSTANT_FALSE);
	}
}

void Compiler::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_LIST, expr.expressions.size());
}

void Compiler::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_TUPLE, expr.expressions.size());
}

void Compiler::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}

	emit(OpCode::MAKE_MAP, expr.pairs.size());
}

void Compiler::visit(UDTConstruct const& expr)
{
}

void Compiler::visit(UDTMemberAccess const& expr)
{
}

void Compiler::visit(EnumMember const& expr)
{
}

void Compiler::visit(Identifier const& expr)
{
	auto scope = scope_stack.top();
	auto label = scope->symbol_table->lookup(expr.name);
	emit(OpCode::LOAD_LOCAL, label);
}

void Compiler::visit(Call const& expr)
{
	wstring function_name = expr.name;

	int label = find_string_constant(function_name);

	if (label == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(function_name));
		label = add_to_constant_pool(move(value));
	}

	visit(expr.arguments);

	int argument_count = expr.arguments.size();
	emit(OpCode::CALL, label, argument_count);
}

void Compiler::visit(Unary const& expr)
{
	visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::BANG:
	{
		emit(OpCode::UNARY_NOT);
		break;
	}
	case WTokenType::UNARY_MINUS:
	{
		emit(OpCode::UNARY_NEGATIVE);
		break;
	}
	case WTokenType::UNARY_PLUS:
	{
		emit(OpCode::UNARY_POSITIVE);
		break;
	}
	default: {
		break;
	}
	}
}

void Compiler::visit(Binary const& expr)
{
	visit(expr.right);
	visit(expr.left);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	{
		emit(OpCode::ADD);
		break;
	}
	case WTokenType::MINUS:
	{
		emit(OpCode::SUBTRACT);
		break;
	}
	case WTokenType::STAR:
	{
		emit(OpCode::MULTIPLY);
		break;
	}
	case WTokenType::DIVISION:
	{
		emit(OpCode::DIVISION);
		break;
	}
	case WTokenType::REMINDER:
	{
		emit(OpCode::REMINDER);
		break;
	}
	case WTokenType::POWER:
	{
		emit(OpCode::POWER);
		break;
	}
	case WTokenType::EQUAL_EQUAL:
	{
		emit(OpCode::EQUAL);
		break;
	}
	case WTokenType::BANG_EQUAL:
	{
		emit(OpCode::NOT_EQUAL);
		break;
	}
	case WTokenType::LESSER_THAN:
	{
		emit(OpCode::LESSER_THAN);
		break;
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		emit(OpCode::LESSER_THAN_EQUAL);
		break;
	}
	case WTokenType::GREATER_THAN:
	{
		emit(OpCode::GREATER_THAN);
		break;
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		emit(OpCode::GREATER_THAN_EQUAL);
		break;
	}
	case WTokenType::AND:
	{
		emit(OpCode::AND);
		break;
	}
	case WTokenType::OR:
	{
		emit(OpCode::OR);
		break;
	}
	default:
	{
		break;
	}
	}
}

// Scope

Scope_ptr Compiler::enter_scope()
{
	if (scope_stack.size() > 0)
	{
		auto scope = scope_stack.top();
		auto enclosing_symbol_table = scope->symbol_table;
		scope_stack.push(make_shared<Scope>(enclosing_symbol_table));
	}
	else
	{
		scope_stack.push(make_shared<Scope>());
	}

	return scope_stack.top();
}

Instructions Compiler::leave_scope()
{
	auto old_outer_scope = scope_stack.top();
	scope_stack.pop();

	Instructions instructions = move(old_outer_scope->instructions);

	if (scope_stack.size() > 0)
	{
		auto new_outer_scope = scope_stack.top();

		new_outer_scope->instructions.insert(
			std::end(new_outer_scope->instructions),
			std::begin(instructions),
			std::end(instructions)
		);
	}

	return instructions;
}

int Compiler::define_variable(wstring name)
{
	int label = next_id++;

	auto scope = scope_stack.top();
	scope->symbol_table->define(name, label);

	id_to_name[label] = name;

	return label;
}

// Emit

void Compiler::emit(Instruction instruction)
{
	auto scope = scope_stack.top();

	scope->instructions.insert(
		std::end(scope->instructions),
		std::begin(instruction),
		std::end(instruction)
	);
}

void Compiler::emit(OpCode opcode)
{
	Instruction instruction = make_instruction(opcode);
	emit(instruction);
}

void Compiler::emit(OpCode opcode, int operand)
{
	Instruction instruction = make_instruction(opcode, operand);
	emit(instruction);
}

void Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	Instruction instruction = make_instruction(opcode, operand_1, operand_2);
	emit(instruction);
}

// Make Instruction

Instruction Compiler::make_instruction(OpCode opcode)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));

	return instruction;
}

Instruction Compiler::make_instruction(OpCode opcode, int operand)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand));

	return instruction;
}

Instruction Compiler::make_instruction(OpCode opcode, int operand_1, int operand_2)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand_1));
	instruction.push_back(static_cast<std::byte>(operand_2));

	return instruction;
}

// Constant Pool

int Compiler::add_to_constant_pool(Object_ptr value)
{
	wstring name = L" ";

	// must remove this ugly thing
	std::visit(overloaded{
		[&](NumberObject const& obj) { name = to_wstring(obj.value); },
		[&](StringObject const& obj) { name = obj.value; },
		[&](BooleanObject const& obj) { name = to_wstring(obj.value); },
		[&](ListObject const& obj) { name = L"ListObject"; },
		[&](TupleObject const& obj) { name = L"TupleObject"; },
		[&](EnumMemberObject const& obj) { name = L"EnumMemberObject"; },
		[&](MapObject const& obj) { name = L"MapObject"; },
		[&](VariantObject const& obj) { name = L"VariantObject"; },
		[&](ReturnObject const& obj) { name = L"ReturnObject"; },
		[&](ErrorObject const& obj) { name = L"ErrorObject"; },
		[&](BuiltInsObject const& obj) { name = L"BuiltInsObject"; },
		[&](NoneObject const& obj) { name = L"NoneObject"; },
		[&](FunctionObject const& obj) { name = L"FunctionObject"; },
		[&](GeneratorObject const& obj) { name = L"GeneratorObject"; },

		[&](auto) { name = L" "; }
		}, *value);

	int label = next_id++;
	constant_pool.insert({ label, value });

	id_to_name[label] = name;

	return label;
}

int Compiler::find_string_constant(wstring text)
{
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[text](const auto& p)
		{
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);
				return x->value == text;
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

int Compiler::find_number_constant(int number)
{
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[number](const auto& p)
		{
			if (holds_alternative<NumberObject>(*p.second))
			{
				NumberObject* x = get_if<NumberObject>(&*p.second);
				return x->value == number;
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		return result->first;
	}

	return -1;
}

// Label

int Compiler::create_label()
{
	int label = next_label++;

	// set dummy target
	targets[label] = 0;

	return label;
}