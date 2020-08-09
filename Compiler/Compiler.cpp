#pragma once
#include "pch.h"
#include "Compiler.h"
#include "Assertion.h"

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define VOID std::make_shared<Object>(ReturnObject())

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	Bytecode_ptr bytecode = std::make_shared<Bytecode>(instructions, constant_pool);
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

		int label = labels.size();
		emit(OpCode::JUMP_IF_FALSE, label);
		labels[label] = 0;

		auto body = branch.second;
		visit(body);

		labels[label] = instructions.size();
	}

	visit(statement.else_block);
}

void Compiler::visit(WhileLoop const& statement)
{
}

void Compiler::visit(ForInLoop const& statement)
{
}

void Compiler::visit(Break const& statement)
{
}

void Compiler::visit(Continue const& statement)
{
}

void Compiler::visit(Pass const& statement)
{
}

void Compiler::visit(Return const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
	else
	{
		add_to_constant_pool(VOID);
	}

	emit(OpCode::RETURN);
}

void Compiler::visit(YieldStatement const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
	else
	{
		add_to_constant_pool(VOID);
	}

	emit(OpCode::YIELD_OP);
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
	emit(OpCode::ARRAY, expr.expressions.size());
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
	case TokenType::BANG:
	{
		emit(OpCode::UNARY_BANG);
		break;
	}
	case TokenType::UNARY_MINUS:
	{
		emit(OpCode::UNARY_SUBTRACT);
		break;
	}
	case TokenType::UNARY_PLUS:
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
	case TokenType::BANG:
	{
		emit(OpCode::UNARY_BANG);
		break;
	}
	case TokenType::PLUS:
	{
		emit(OpCode::BINARY_ADD);
		break;
	}
	case TokenType::MINUS:
	{
		emit(OpCode::BINARY_SUBTRACT);
		break;
	}
	case TokenType::STAR:
	{
		emit(OpCode::BINARY_MULTIPLY);
		break;
	}
	case TokenType::DIVISION:
	{
		emit(OpCode::BINARY_DIVISION);
		break;
	}
	case TokenType::REMINDER:
	{
		emit(OpCode::BINARY_REMINDER);
		break;
	}
	case TokenType::POWER:
	{
		emit(OpCode::BINARY_POWER);
		break;
	}
	case TokenType::LESSER_THAN:
	{
		emit(OpCode::BINARY_LESSER_THAN);
		break;
	}
	case TokenType::LESSER_THAN_EQUAL:
	{
		emit(OpCode::BINARY_LESSER_THAN_EQUAL);
		break;
	}
	case TokenType::GREATER_THAN:
	{
		emit(OpCode::BINARY_GREATER_THAN);
		break;
	}
	case TokenType::GREATER_THAN_EQUAL:
	{
		emit(OpCode::BINARY_GREATER_THAN_EQUAL);
		break;
	}
	case TokenType::AND:
	{
		emit(OpCode::BINARY_AND);
		break;
	}
	case TokenType::OR:
	{
		emit(OpCode::BINARY_OR);
		break;
	}
	default: {
		break;
	}
	}
}

// Emit

int Compiler::emit(OpCode opcode)
{
	auto instruction = make_instruction(opcode);
	int position = instructions.size();

	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);

	return position;
}

int Compiler::emit(OpCode opcode, int operand)
{
	auto instruction = make_instruction(opcode, operand);
	int position = instructions.size();

	instructions.insert(
		std::end(instructions),
		std::begin(instruction),
		std::end(instruction)
	);

	return position;
}

int Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	auto instruction = make_instruction(opcode, operand_1, operand_2);
	int position = instructions.size();

	instructions.insert(
		std::end(instructions),
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