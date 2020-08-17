#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::wstring;
using std::holds_alternative;
using std::get_if;

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	Bytecode_ptr bytecode = std::make_shared<Bytecode>(
		scopes.top()->instructions,
		constant_pool,
		relative_jumps
		);

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
		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Pass const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](YieldStatement const& stat) { visit(stat); },
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

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& block)
{
	if (block.size() == 0)
	{
		emit(OpCode::PASS);
		return;
	}

	for (const auto stat : block)
	{
		visit(stat);
	}
}

void Compiler::visit(Assignment const& statement)
{
	int id = symbol_table->lookup(statement.name);
	visit(statement.expression);

	emit(OpCode::SET_VARIABLE, id);
}

void Compiler::visit(Branching const& statement)
{
	int exit_tree_label = create_label();
	int exit_tree_relative_jump = 0;

	// Conditional branches

	for (const auto branch : statement.branches)
	{
		auto condition = branch.first;
		visit(condition);

		enter_scope();

		int exit_branch_label = create_label();
		emit(OpCode::JUMP_IF_FALSE, exit_branch_label);

		auto body = branch.second;
		visit(&body);

		emit(OpCode::POP); // Pop condition
		emit(OpCode::JUMP, exit_tree_label);

		Instructions instructions = leave_scope();

		int instructions_length = instructions.size();
		set_label(exit_branch_label, instructions_length);

		exit_tree_relative_jump += instructions_length;
	}

	// Else Branch

	enter_scope();
	visit(&statement.else_block);
	Instructions instructions = leave_scope();

	int instructions_length = instructions.size();
	exit_tree_relative_jump += instructions_length;

	relative_jumps[exit_tree_label] = exit_tree_relative_jump;
}

void Compiler::visit(WhileLoop const& statement)
{
	auto condition = statement.condition;
	visit(condition);

	enter_scope();

	int block_begin_label = create_label();
	int block_end_label = create_label();

	emit(OpCode::JUMP_IF_FALSE, block_end_label);

	auto body = statement.block;
	visit(&body);

	emit(OpCode::JUMP, block_begin_label);

	Instructions instructions = leave_scope();
	int instructions_length = instructions.size();

	set_label(block_end_label, instructions_length);
	set_label(block_begin_label, -instructions_length);
}

void Compiler::visit(ForInLoop const& statement)
{
	int block_begin_label = create_label();
	int block_end_label = create_label();

	enter_scope();

	Expression_ptr iterable = statement.iterable;
	visit(iterable);

	int item_id = next_id++;
	symbol_table->define(statement.item_name, item_id);

	std::visit(overloaded{
		[&](wstring const& expr) { emit(OpCode::ITERATE_OVER_STRING, item_id); },
		[&](ListLiteral const& expr) { emit(OpCode::ITERATE_OVER_LIST, item_id); },
		[&](MapLiteral const& expr) { emit(OpCode::ITERATE_OVER_MAP, item_id); },
		[&](Identifier const& expr) { emit(OpCode::ITERATE_OVER_IDENTIFIER, item_id); },

		[](auto) { FATAL("Not an iterable!"); }
		}, *iterable);

	emit(OpCode::JUMP_IF_FALSE, block_end_label);

	auto body = statement.block;
	visit(&body);

	emit(OpCode::JUMP, block_begin_label);

	Instructions instructions = leave_scope();
	int instructions_length = instructions.size();

	set_label(block_end_label, instructions_length + 2);
	set_label(block_begin_label, -instructions_length + 2);
}

void Compiler::visit(Break const& statement)
{
	auto block_skip_label = scopes.top()->break_label;
	emit(OpCode::JUMP, block_skip_label);
}

void Compiler::visit(Continue const& statement)
{
	auto block_begin_label = scopes.top()->continue_label;
	emit(OpCode::JUMP, block_begin_label);
}

void Compiler::visit(Pass const& statement)
{
	emit(OpCode::PASS);
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

	int id = next_id;
	next_id++;

	symbol_table->define(statement.name, id);

	emit(OpCode::SET_VARIABLE, id);
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
		int id = next_id;
		symbol_table->define(statement.name, id);
		emit(OpCode::SET_VARIABLE, id);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(instructions));

	int id = add_to_constant_pool(move(function_object));
	emit(OpCode::CONSTANT, id);
}

void Compiler::visit(GeneratorDefinition const& statement)
{
	enter_scope();

	for (const auto [arg_name, _] : statement.arguments)
	{
		int id = next_id;
		symbol_table->define(statement.name, id);
		emit(OpCode::SET_VARIABLE, id);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorObject(instructions));

	int id = add_to_constant_pool(move(generator_object));
	emit(OpCode::CONSTANT, id);
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
	emit(OpCode::POP);
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
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[number](const auto& p)
		{
			if (holds_alternative<NumberObject>(*p.second))
			{
				NumberObject* x = get_if<NumberObject>(&*p.second);

				if (x->value == number)
				{
					return true;
				}
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		int id = result->first;
		emit(OpCode::CONSTANT, id);
	}
	else
	{
		auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
		int id = add_to_constant_pool(move(value));
		emit(OpCode::CONSTANT, id);
	}
}

void Compiler::visit(const std::wstring text)
{
	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[text](const auto& p)
		{
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);

				if (x->value == text)
				{
					return true;
				}
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		int id = result->first;
		emit(OpCode::CONSTANT, id);
	}
	else
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(text));
		int id = add_to_constant_pool(move(value));
		emit(OpCode::CONSTANT, id);
	}
}

void Compiler::visit(const bool boolean)
{
	if (boolean)
	{
		emit(OpCode::CONSTANT_TRUE);
	}
	else
	{
		emit(OpCode::CONSTANT_FALSE);
	}
}

void Compiler::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::LIST, expr.expressions.size());
}

void Compiler::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::TUPLE, expr.expressions.size());
}

void Compiler::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}

	emit(OpCode::MAP, expr.pairs.size());
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
	auto id = symbol_table->lookup(expr.name);
	emit(OpCode::GET_VARIABLE, id);
}

void Compiler::visit(Call const& expr)
{
	wstring function_name = expr.name;

	auto result = std::find_if(
		constant_pool.begin(),
		constant_pool.end(),
		[function_name](const auto& p)
		{
			if (holds_alternative<StringObject>(*p.second))
			{
				StringObject* x = get_if<StringObject>(&*p.second);

				if (x->value == function_name)
				{
					return true;
				}
			}

			return false;
		}
	);

	if (result != constant_pool.end())
	{
		int id = result->first;
		emit(OpCode::CONSTANT, id);
	}
	else
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(function_name));
		int id = add_to_constant_pool(move(value));
		emit(OpCode::CONSTANT, id);
	}

	visit(expr.arguments);
	int argument_count = expr.arguments.size();
	emit(OpCode::CALL_FUNCTION, argument_count);
}

void Compiler::visit(Unary const& expr)
{
	visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::BANG:
	{
		emit(OpCode::UNARY_BANG);
		break;
	}
	case WTokenType::UNARY_MINUS:
	{
		emit(OpCode::UNARY_SUBTRACT);
		break;
	}
	case WTokenType::UNARY_PLUS:
	{
		emit(OpCode::UNARY_ADD);
		break;
	}
	default: {
		break;
	}
	}
}

void Compiler::visit(Binary const& expr)
{
	visit(expr.left);
	visit(expr.right);

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

// Labels

int Compiler::create_label()
{
	int label = relative_jumps.size();
	relative_jumps.push_back(0);

	return label;
}

void Compiler::set_label(int label, int relative_jump)
{
	relative_jumps[label] = relative_jump;
}

// Scope

void Compiler::enter_scope()
{
	auto scope = std::make_shared<CompilationScope>();
	scopes.push(move(scope));

	symbol_table = std::make_shared<CSymbolTable>(symbol_table);
}

Instructions Compiler::leave_scope()
{
	// Pop Scope

	auto scope = scopes.top();
	scopes.pop();

	// Remove associated symbol table

	if (symbol_table->enclosing_scope.has_value())
	{
		symbol_table = symbol_table->enclosing_scope.value();
	}

	return scope->instructions;
}

// Emit

void Compiler::emit(Instruction instruction)
{
	auto scope = scopes.top();

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

// Utils

int Compiler::add_to_constant_pool(Object_ptr value)
{
	int id = next_id;
	next_id++;

	constant_pool.insert({ id, move(value) });

	return id;
}