#pragma once
#include "pch.h"
#include "CFGBuilder.h"
#include "CFG.h"
#include "CFG_Node.h"
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
#define MAKE_CFG_NODE(x) std::make_shared<CFGNode>(x)
#define TOP_CONTEXT context_stack.top()

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::wstring;
using std::make_shared;
using std::holds_alternative;
using std::get_if;
using std::pair;
using std::nullopt;
using std::vector;

CFG_ptr CFGBuilder::execute(const Module_ptr module_ast)
{
	next_id = 0;
	symbol_table = make_shared<CSymbolTable>();
	module_cfg = make_shared<CFG>();

	enter_context();

	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	return module_cfg;
}

// Statement

void CFGBuilder::visit(const Statement_ptr statement)
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

void CFGBuilder::visit(std::vector<Statement_ptr> const& block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
}

void CFGBuilder::visit(Assignment const& statement)
{
	visit(statement.expression);

	int id = symbol_table->lookup(statement.name);
	emit(OpCode::STORE_LOCAL, id);
}

void CFGBuilder::visit(Branching const& statement)
{
	set_end_node(MAKE_CFG_NODE(CFGNodeType::Terminal));

	CFGNode_ptr first_condition;
	CFGNode_ptr last_condition_node;
	CFGNode_ptr last_basic_block;

	for (const auto branch : statement.branches)
	{
		enter_context();

		auto condition_node = MAKE_CFG_NODE(CFGNodeType::Condition);
		set_current_node(condition_node);
		visit(&branch.first);

		auto basic_block = MAKE_CFG_NODE(CFGNodeType::Basic_Block);
		TOP_CONTEXT->condition_node->true_outgoing_link = basic_block;
		set_current_node(basic_block);

		enter_scope();
		visit(&branch.second);
		leave_scope();

		TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;

		if (last_condition_node)
		{
			last_condition_node->false_outgoing_link = condition_node;
		}

		last_condition_node = condition_node;

		last_basic_block = TOP_CONTEXT->current_node;

		if (first_condition == nullptr)
		{
			first_condition = condition_node;
		}

		leave_context();
	}

	// compile else block

	std::optional<CFGNode_ptr> else_basic_block = nullopt;

	if (statement.else_block.size() > 0)
	{
		enter_context();
		else_basic_block = MAKE_CFG_NODE(CFGNodeType::Basic_Block);
		set_current_node(else_basic_block.value());
		enter_scope();
		visit(&statement.else_block);
		leave_scope();
		TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;
		leave_context();
	}

	last_condition_node->false_outgoing_link = else_basic_block;
	last_basic_block->false_outgoing_link = else_basic_block;

	module_cfg->extend(first_condition, TOP_CONTEXT->end_node);
}

void CFGBuilder::visit(WhileLoop const& statement)
{
	set_end_node(MAKE_CFG_NODE(CFGNodeType::Terminal));

	set_condition_node(MAKE_CFG_NODE(CFGNodeType::Condition));
	TOP_CONTEXT->condition_node->false_outgoing_link = TOP_CONTEXT->end_node;
	set_current_node(TOP_CONTEXT->condition_node);

	enter_scope();
	visit(statement.condition);
	leave_scope();

	set_current_node(MAKE_CFG_NODE(CFGNodeType::Basic_Block));
	TOP_CONTEXT->condition_node->false_outgoing_link = TOP_CONTEXT->current_node;

	visit(&statement.block);

	TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;

	module_cfg->extend(TOP_CONTEXT->condition_node, TOP_CONTEXT->end_node);
}

void CFGBuilder::visit(ForInLoop const& statement)
{
}

void CFGBuilder::visit(Pass const& statement)
{
	emit(OpCode::NO_OP);
	leave_context();
}

void CFGBuilder::visit(Break const& statement)
{
	emit(OpCode::JUMP_ABSOLUTE, 0);
	TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;
	leave_context();
}

void CFGBuilder::visit(Continue const& statement)
{
	emit(OpCode::JUMP_ABSOLUTE, 0);
	TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->condition_node;
	leave_context();
}

void CFGBuilder::visit(Return const& statement)
{
	TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;

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

void CFGBuilder::visit(YieldStatement const& statement)
{
	TOP_CONTEXT->current_node->outgoing_link = TOP_CONTEXT->end_node;

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

void CFGBuilder::visit(VariableDefinition const& statement)
{
	visit(statement.expression);

	int id = next_id++;
	symbol_table->define(statement.name, id);

	emit(OpCode::STORE_LOCAL, id);
}

void CFGBuilder::visit(UDTDefinition const& statement)
{
}

void CFGBuilder::visit(AliasDefinition const& statement)
{
}

void CFGBuilder::visit(FunctionDefinition const& statement)
{
	enter_scope();

	for (const auto [arg_name, _] : statement.arguments)
	{
		int id = next_id++;
		symbol_table->define(statement.name, id);
	}

	visit(&statement.block);

	leave_scope();
	int parameter_count = statement.arguments.size();

	/*auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(instructions, parameter_count));

	int id = add_to_constant_pool(move(function_object));
	emit(OpCode::PUSH_CONSTANT, id);*/
}

void CFGBuilder::visit(GeneratorDefinition const& statement)
{
}

void CFGBuilder::visit(EnumDefinition const& statement)
{
}

void CFGBuilder::visit(ImportCustom const& statement)
{
}

void CFGBuilder::visit(ImportInBuilt const& statement)
{
}

void CFGBuilder::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::POP_FROM_STACK);
}

void CFGBuilder::visit(AssertStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::ASSERT);
}

// Expression

void CFGBuilder::visit(const Expression_ptr expression)
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

void CFGBuilder::visit(std::vector<Expression_ptr> const& expressions)
{
	for (const auto expr : expressions)
	{
		visit(expr);
	}
}

void CFGBuilder::visit(const double number)
{
	int id = find_number_constant(number);

	if (id == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(NumberObject(number));
		id = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, id);
}

void CFGBuilder::visit(const std::wstring text)
{
	int id = find_string_constant(text);

	if (id == -1)
	{
		auto value = MAKE_OBJECT_VARIANT(StringObject(text));
		id = add_to_constant_pool(move(value));
	}

	emit(OpCode::PUSH_CONSTANT, id);
}

void CFGBuilder::visit(const bool boolean)
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

void CFGBuilder::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_LIST, expr.expressions.size());
}

void CFGBuilder::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_TUPLE, expr.expressions.size());
}

void CFGBuilder::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}

	emit(OpCode::MAKE_MAP, expr.pairs.size());
}

void CFGBuilder::visit(UDTConstruct const& expr)
{
}

void CFGBuilder::visit(UDTMemberAccess const& expr)
{
}

void CFGBuilder::visit(EnumMember const& expr)
{
}

void CFGBuilder::visit(Identifier const& expr)
{
	auto id = symbol_table->lookup(expr.name);
	emit(OpCode::LOAD_LOCAL, id);
}

void CFGBuilder::visit(Call const& expr)
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

void CFGBuilder::visit(Unary const& expr)
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

void CFGBuilder::visit(Binary const& expr)
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

void CFGBuilder::enter_scope()
{
	symbol_table = std::make_shared<CSymbolTable>(symbol_table);
}

void CFGBuilder::leave_scope()
{
	if (symbol_table->enclosing_scope.has_value())
	{
		symbol_table = symbol_table->enclosing_scope.value();
	}
}

// CFGBuilder Context

void CFGBuilder::enter_context()
{
	auto context = make_shared<CFGContext>();
	context->condition_node = MAKE_CFG_NODE(CFGNodeType::Condition);
	context->current_node = MAKE_CFG_NODE(CFGNodeType::Basic_Block);
	context->end_node = MAKE_CFG_NODE(CFGNodeType::Terminal);

	context_stack.push(context);
}

void CFGBuilder::leave_context()
{
	context_stack.pop();
}

void CFGBuilder::set_current_node(CFGNode_ptr node)
{
	TOP_CONTEXT->current_node = move(node);
}

void CFGBuilder::set_condition_node(CFGNode_ptr node)
{
	TOP_CONTEXT->condition_node = move(node);
}

void CFGBuilder::set_end_node(CFGNode_ptr node)
{
	TOP_CONTEXT->end_node = move(node);
}

// Emit

void CFGBuilder::emit(Instruction instruction)
{
	auto d = TOP_CONTEXT->current_node->instructions;

	if (TOP_CONTEXT->current_node->instructions.size() > 0)
	{
		TOP_CONTEXT->current_node->instructions.insert(
			std::end(TOP_CONTEXT->current_node->instructions),
			std::begin(instruction),
			std::end(instruction)
		);
	}
	else
	{
		TOP_CONTEXT->current_node->instructions = instruction;
	}
}

void CFGBuilder::emit(OpCode opcode)
{
	Instruction instruction = make_instruction(opcode);
	emit(instruction);
}

void CFGBuilder::emit(OpCode opcode, int operand)
{
	Instruction instruction = make_instruction(opcode, operand);
	emit(instruction);
}

void CFGBuilder::emit(OpCode opcode, int operand_1, int operand_2)
{
	Instruction instruction = make_instruction(opcode, operand_1, operand_2);
	emit(instruction);
}

// Make Instruction

Instruction CFGBuilder::make_instruction(OpCode opcode)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));

	return instruction;
}

Instruction CFGBuilder::make_instruction(OpCode opcode, int operand)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand));

	return instruction;
}

Instruction CFGBuilder::make_instruction(OpCode opcode, int operand_1, int operand_2)
{
	Instruction instruction;
	instruction.push_back(static_cast<std::byte>(opcode));
	instruction.push_back(static_cast<std::byte>(operand_1));
	instruction.push_back(static_cast<std::byte>(operand_2));

	return instruction;
}

// Constant Pool

int CFGBuilder::add_to_constant_pool(Object_ptr value)
{
	int id = next_id++;
	constant_pool.insert({ id, move(value) });

	return id;
}

int CFGBuilder::find_string_constant(wstring text)
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

int CFGBuilder::find_number_constant(int number)
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