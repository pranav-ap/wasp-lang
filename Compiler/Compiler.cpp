#pragma once
#include "pch.h"
#include "Compiler.h"
#include "CFG.h"
#include "CFG_Node.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <variant>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)
#define MAKE_CFG_NODE_VARIANT(x) std::make_shared<CFG_Node>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::wstring;
using std::make_shared;
using std::holds_alternative;
using std::get_if;

Bytecode_ptr Compiler::execute(const Module_ptr module_ast)
{
	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	Bytecode_ptr bytecode;
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

		[](auto) { FATAL("Unexpected statement!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& block)
{
	if (block.size() == 0)
	{
		emit(OpCode::NO_OP);
		return;
	}

	for (const auto stat : block)
	{
		visit(stat);
	}
}

void Compiler::visit(Assignment const& statement)
{
	visit(statement.expression);

	int id = symbol_table->lookup(statement.name);
	emit(OpCode::STORE_LOCAL, id);
}

void Compiler::visit(Branching const& statement)
{
	std::vector<std::pair<Condition_Node, Basic_Block_ptr>> pairs;

	for (const auto branch : statement.branches)
	{
		auto condition = branch.first;
		visit(condition);

		auto condition_node = make_shared<Condition_Node>();
		condition_node->push(make_instruction(OpCode::JUMP_ABSOLUTE_IF_FALSE, 0));

		enter_scope();

		auto block = branch.second;
		auto basic_block = make_shared<Basic_Block>();

		for (const auto stat : block)
		{
			bool skip = true;

			std::visit(overloaded{
				[&](Break const& stat) { visit(stat); },
				[&](Continue const& stat) { visit(stat); },
				[&](Pass const& stat) { visit(stat); },
				[&](Return const& stat) { visit(stat); },
				[&](YieldStatement const& stat) { visit(stat); },

				[&](auto) { skip = false; }
				}, *stat);

			if (!skip)
			{
				visit(stat);
			}
		}

		basic_block->push(make_instruction(OpCode::JUMP_ABSOLUTE, 0));

		leave_scope();
	}

	// create unconditional block

	// for each (condi, uncondi) in vector
	//		condi.true = uncondi
	//		condi.false = next_condi / else block / nullopt
	//		uncondi.out = end_node

	// start_node = vector[0].condi

	// end_node
}

void Compiler::visit(WhileLoop const& statement)
{
	auto condition_node = make_shared<Condition_Node>();

	auto basic_block = make_shared<Basic_Block>();
	basic_block->outgoing_link = MAKE_CFG_NODE_VARIANT(condition_node);

	auto end_node = make_shared<Terminal_Node>();
	condition_node->false_outgoing_link = MAKE_CFG_NODE_VARIANT(end_node);
}

void Compiler::visit(ForInLoop const& statement)
{
}

void Compiler::visit(Pass const& statement)
{
	emit(OpCode::NO_OP);
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

	int id = next_id++;
	symbol_table->define(statement.name, id);

	emit(OpCode::STORE_LOCAL, id);
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
		int id = next_id++;
		symbol_table->define(statement.name, id);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	int parameter_count = statement.arguments.size();

	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(instructions, parameter_count));

	int id = add_to_constant_pool(move(function_object));
	emit(OpCode::PUSH_CONSTANT, id);
}

void Compiler::visit(GeneratorDefinition const& statement)
{
	enter_scope();

	for (const auto [arg_name, _] : statement.arguments)
	{
		int id = next_id++;
		symbol_table->define(statement.name, id);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	int parameter_count = statement.arguments.size();

	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorObject(instructions, parameter_count));

	int id = add_to_constant_pool(move(generator_object));
	emit(OpCode::PUSH_CONSTANT, id);
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
	int id = find_number_constant(number);

	if (id == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
		id = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, id);
}

void Compiler::visit(const std::wstring text)
{
	int id = find_string_constant(text);

	if (id == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(text));
		id = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, id);
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
	auto id = symbol_table->lookup(expr.name);
	emit(OpCode::LOAD_LOCAL, id);
}

void Compiler::visit(Call const& expr)
{
	wstring function_name = expr.name;

	int id = find_string_constant(function_name);

	if (id == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(function_name));
		id = add_to_constant_pool(move(value));
	}

	visit(expr.arguments);

	int argument_count = expr.arguments.size();
	emit(OpCode::CALL, id, argument_count);
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

// Scope

void Compiler::enter_scope()
{
	symbol_table = std::make_shared<CSymbolTable>(symbol_table);
}

void Compiler::leave_scope()
{
	if (symbol_table->enclosing_scope.has_value())
	{
		symbol_table = symbol_table->enclosing_scope.value();
	}
}

// Emit

void Compiler::emit(OpCode opcode)
{
	Instruction instruction = make_instruction(opcode);
	local_cfg->emit(instruction);
}

void Compiler::emit(OpCode opcode, int operand)
{
	Instruction instruction = make_instruction(opcode, operand);
	local_cfg->emit(instruction);
}

void Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	Instruction instruction = make_instruction(opcode, operand_1, operand_2);
	local_cfg->emit(instruction);
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
	int id = next_id++;
	constant_pool.insert({ id, move(value) });

	return id;
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