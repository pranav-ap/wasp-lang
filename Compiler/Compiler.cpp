#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Assertion.h"
#include <memory>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	Bytecode_ptr bytecode = std::make_shared<Bytecode>(scopes.top()->instructions, constant_pool);
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
	visit(statement.lhs_expressions);
	visit(statement.rhs_expressions);
}

void Compiler::visit(Branching const& statement)
{
	for (const auto branch : statement.branches)
	{
		auto condition = branch.first;
		visit(condition);

		enter_scope();

		int exit_branch_label = set_exit_block_label();
		emit(OpCode::JUMP_IF_FALSE, exit_branch_label);

		auto body = branch.second;
		visit(&body);

		emit(OpCode::POP); // Pop condition

		save_branch_size(exit_branch_label);

		leave_scope();
	}

	// Else Branch

	enter_scope();

	visit(&statement.else_block);

	int exit_branch_label = set_exit_block_label();
	save_branch_size(exit_branch_label);

	leave_scope();

	// Set On Truthy Jump labels

	auto scope = scopes.top();

	while (scope->branch_sizes.size() > 0)
	{
	}

	scopes.top()->branch_sizes.empty();
}

void Compiler::visit(WhileLoop const& statement)
{
	auto condition = statement.condition;
	visit(condition);

	enter_scope();

	int start_loop_label = set_start_block_label();
	int exit_loop_label = set_exit_block_label();

	emit(OpCode::JUMP_IF_FALSE, exit_loop_label);

	auto body = statement.block;
	visit(&body);

	jump_locations[exit_loop_label] = scopes.top()->instructions.size();

	emit(OpCode::JUMP, start_loop_label);

	leave_scope();
}

void Compiler::visit(ForInLoop const& statement)
{
}

void Compiler::visit(Break const& statement)
{
	auto exit_loop_label = scopes.top()->exit_loop_label;
	emit(OpCode::JUMP, exit_loop_label);
}

void Compiler::visit(Continue const& statement)
{
	auto start_loop_label = scopes.top()->start_loop_label;
	emit(OpCode::JUMP, start_loop_label);
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
	emit(OpCode::SET_VARIABLE);

	auto symbol = symbol_table->define(statement.name);
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
	visit(&statement.block);

	auto instructions = leave_scope();
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(instructions));

	int id = add_to_constant_pool(function_object);
	emit(OpCode::CONSTANT, id);
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
	emit(OpCode::POP);
}

void Compiler::visit(AssertStatement const& statement)
{
	visit(statement.expression);
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
	auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
	int id = add_to_constant_pool(value);
	emit(OpCode::CONSTANT, id);
}

void Compiler::visit(const std::wstring text)
{
	auto value = MAKE_OBJECT_VARIANT(StringObject(text));
	int id = add_to_constant_pool(value);
	emit(OpCode::CONSTANT, id);
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

	emit(OpCode::MAP, expr.pairs.size() * 2);
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
	auto symbol = symbol_table->lookup(expr.name);
	emit(OpCode::GET_VARIABLE, symbol->id);
}

void Compiler::visit(Call const& expr)
{
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
		emit(OpCode::BINARY_ADD);
		break;
	}
	case WTokenType::MINUS:
	{
		emit(OpCode::BINARY_SUBTRACT);
		break;
	}
	case WTokenType::STAR:
	{
		emit(OpCode::BINARY_MULTIPLY);
		break;
	}
	case WTokenType::DIVISION:
	{
		emit(OpCode::BINARY_DIVISION);
		break;
	}
	case WTokenType::REMINDER:
	{
		emit(OpCode::BINARY_REMINDER);
		break;
	}
	case WTokenType::POWER:
	{
		emit(OpCode::BINARY_POWER);
		break;
	}
	case WTokenType::LESSER_THAN:
	{
		emit(OpCode::BINARY_LESSER_THAN);
		break;
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		emit(OpCode::BINARY_LESSER_THAN_EQUAL);
		break;
	}
	case WTokenType::GREATER_THAN:
	{
		emit(OpCode::BINARY_GREATER_THAN);
		break;
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		emit(OpCode::BINARY_GREATER_THAN_EQUAL);
		break;
	}
	case WTokenType::AND:
	{
		emit(OpCode::BINARY_AND);
		break;
	}
	case WTokenType::OR:
	{
		emit(OpCode::BINARY_OR);
		break;
	}
	default: {
		break;
	}
	}
}

// Labels

int Compiler::set_start_block_label()
{
	int start_label = jump_locations.size();
	scopes.top()->start_loop_label = start_label;
	jump_locations[start_label] = scopes.top()->instructions.size();

	return start_label;
}

int Compiler::set_exit_block_label()
{
	int exit_label = jump_locations.size();
	scopes.top()->exit_loop_label = exit_label;
	jump_locations[exit_label] = -1;

	return exit_label;
}

void Compiler::save_branch_size(int exit_branch_label)
{
	auto scope = scopes.top();
	scope->branch_sizes.push({ exit_branch_label, scope->instructions.size() });
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
	auto scope = scopes.top();
	scopes.pop();

	if (symbol_table->enclosing_scope.has_value())
	{
		symbol_table = symbol_table->enclosing_scope.value();
	}

	return scope->instructions;
}

// Emit

int Compiler::emit(OpCode opcode)
{
	auto instruction = make_instruction(opcode);
	int position = scopes.top()->instructions.size();

	scopes.top()->instructions.insert(
		std::end(scopes.top()->instructions),
		std::begin(instruction),
		std::end(instruction)
	);

	return position;
}

int Compiler::emit(OpCode opcode, int operand)
{
	auto instruction = make_instruction(opcode, operand);
	int position = scopes.top()->instructions.size();

	scopes.top()->instructions.insert(
		std::end(scopes.top()->instructions),
		std::begin(instruction),
		std::end(instruction)
	);

	return position;
}

int Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	auto instruction = make_instruction(opcode, operand_1, operand_2);
	int position = scopes.top()->instructions.size();

	scopes.top()->instructions.insert(
		std::end(scopes.top()->instructions),
		std::begin(instruction),
		std::end(instruction)
	);

	return position;
}

// Utils

int Compiler::add_to_constant_pool(Object_ptr value)
{
	constant_pool.push_back(value);
	int index = constant_pool.size() - 1;
	return index;
}

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