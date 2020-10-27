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
using std::make_shared;
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;

void Compiler::execute(const File_ptr ast)
{
	next_label = 0;

	enter_scope();

	memory->get_code_section()->emit(OpCode::START);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	memory->get_code_section()->emit(OpCode::STOP);

	leave_scope();
}

// Statement

void Compiler::visit(const Statement_ptr statement)
{
	std::visit(overloaded{
		[&](IfBranch const& stat) { visit(stat); },
		[&](ElseBranch const& stat) { visit(stat); },
		[&](WhileLoop const& stat) { visit(stat); },
		[&](ForInLoop const& stat) { visit(stat); },
		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](YieldStatement const& stat) { visit(stat); },
		[&](VariableDefinition const& stat) { visit(stat); },
		[&](ClassDefinition const& stat) { visit(stat); },
		[&](InterfaceDefinition const& stat) { visit(stat); },
		[&](AliasDefinition const& stat) { visit(stat); },
		[&](FunctionMethodDefinition const& stat) { visit(stat); },
		[&](GeneratorMethodDefinition const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },
		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](Assert const& stat) { visit(stat); },
		[&](Implore const& stat) { visit(stat); },
		[&](Swear const& stat) { visit(stat); },
		[&](Namespace const& stat) { visit(stat); },
		[&](InfixOperatorDefinition const& stat) { visit(stat); },
		[&](PrefixOperatorDefinition const& stat) { visit(stat); },
		[&](PostfixOperatorDefinition const& stat) { visit(stat); },

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

void Compiler::visit(IfBranch const& statement)
{
	int exit_tree_label = create_label();
	int	branch_label = create_label();

	visit(statement, exit_tree_label, branch_label);

	memory->get_code_section()->emit(OpCode::LABEL, exit_tree_label);
}

void Compiler::visit(IfBranch const& statement, int exit_tree_label, int branch_label)
{
	memory->get_code_section()->emit(OpCode::LABEL, branch_label);
	visit(statement.test);

	auto alternative = statement.alternative;

	if (alternative.has_value())
	{
		if (
			holds_alternative<IfBranch>(*alternative.value()) ||
			holds_alternative<ElseBranch>(*alternative.value())
			)
		{
			branch_label = create_label();
			memory->get_code_section()->emit(OpCode::POP_JUMP_IF_FALSE, branch_label);
		}
		else
		{
			FATAL("Alternative must be an IfBranch or ElseBranch");
		}
	}
	else
	{
		memory->get_code_section()->emit(OpCode::POP_JUMP_IF_FALSE, exit_tree_label);
	}

	enter_scope();
	visit(statement.body);
	memory->get_code_section()->emit(OpCode::POP_JUMP, exit_tree_label);
	leave_scope();

	if (alternative.has_value())
	{
		if (holds_alternative<IfBranch>(*alternative.value()))
		{
			branch_label = create_label();
			auto if_branch = get_if<IfBranch>(&*alternative.value());
			visit(*if_branch, exit_tree_label, branch_label);
		}
		else if (holds_alternative<ElseBranch>(*alternative.value()))
		{
			auto else_branch = get_if<ElseBranch>(&*alternative.value());
			visit(*else_branch, exit_tree_label);
		}
		else
		{
			FATAL("Alternative must be an IfBranch or ElseBranch");
		}
	}
}

void Compiler::visit(ElseBranch const& statement)
{
	FATAL("Else must be part of an if branch");
}

void Compiler::visit(ElseBranch const& statement, int exit_tree_label)
{
	enter_scope();
	memory->get_code_section()->emit(OpCode::LABEL, exit_tree_label);
	visit(statement.body);
	leave_scope();
}

void Compiler::visit(Assignment const& statement)
{
	visit(statement.rhs_expression);

	ASSERT(holds_alternative<TypePattern>(*statement.lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*statement.lhs_expression);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	auto scope = scope_stack.top();
	int label = scope->symbol_table->lookup(identifier->name);
	memory->get_code_section()->emit(OpCode::STORE_LOCAL, label);
}

void Compiler::visit(WhileLoop const& statement)
{
	int condition_label = create_label();
	memory->get_code_section()->emit(OpCode::LABEL, condition_label);

	auto condition = statement.expression;
	visit(condition);

	auto scope = enter_scope();
	scope->continue_label = condition_label;

	int block_end_label = create_label();
	memory->get_code_section()->emit(OpCode::POP_JUMP_IF_FALSE, block_end_label);
	scope->break_label = block_end_label;

	auto body = statement.block;
	visit(body);

	memory->get_code_section()->emit(OpCode::JUMP, condition_label);
	memory->get_code_section()->emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(ForInLoop const& statement)
{
	int iterate_label = create_label();
	memory->get_code_section()->emit(OpCode::LABEL, iterate_label);

	// Place iterable on stack

	auto iterable = statement.rhs_expression;
	visit(iterable);

	auto scope = enter_scope();

	int block_begin_label = create_label();
	memory->get_code_section()->emit(OpCode::LABEL, block_begin_label);
	scope->continue_label = block_begin_label;

	int block_end_label = create_label();
	scope->break_label = block_end_label;

	std::visit(overloaded{
		[&](wstring const& expr) { memory->get_code_section()->emit(OpCode::ITERATE_OVER_STRING, block_end_label); },
		[&](ListLiteral const& expr) { memory->get_code_section()->emit(OpCode::ITERATE_OVER_LIST, block_end_label); },
		[&](MapLiteral const& expr) { memory->get_code_section()->emit(OpCode::ITERATE_OVER_MAP, block_end_label); },
		[&](Identifier const& expr) { memory->get_code_section()->emit(OpCode::ITERATE_OVER_IDENTIFIER, block_end_label); },

		[](auto) { FATAL("Not an iterable!"); }
		}, *iterable);

	ASSERT(holds_alternative<TypePattern>(*statement.lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*statement.lhs_expression);
	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	int item_id = define_variable(identifier->name);
	memory->get_code_section()->emit(OpCode::STORE_LOCAL, item_id);

	auto body = statement.block;
	visit(body);

	memory->get_code_section()->emit(OpCode::JUMP, block_begin_label);
	memory->get_code_section()->emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(Break const& statement)
{
	auto scope = scope_stack.top();
	memory->get_code_section()->emit(OpCode::JUMP, scope->break_label);
}

void Compiler::visit(Continue const& statement)
{
	auto scope = scope_stack.top();
	memory->get_code_section()->emit(OpCode::JUMP, scope->continue_label);
}

void Compiler::visit(Return const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		memory->get_code_section()->emit(OpCode::RETURN_VALUE);
	}
	else
	{
		memory->get_code_section()->emit(OpCode::RETURN_VOID);
	}
}

void Compiler::visit(YieldStatement const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		memory->get_code_section()->emit(OpCode::YIELD_VALUE);
	}
	else
	{
		memory->get_code_section()->emit(OpCode::YIELD_VOID);
	}
}

void Compiler::visit(VariableDefinition const& statement)
{
	visit(statement.expression);

	ASSERT(holds_alternative<Assignment>(*statement.expression), "Must be an Assignment");
	auto assignment = get_if<Assignment>(&*statement.expression);

	ASSERT(holds_alternative<TypePattern>(*assignment->lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*assignment->lhs_expression);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	int id = define_variable(identifier->name);
	memory->get_code_section()->emit(OpCode::STORE_LOCAL, id);
}

void Compiler::visit(ClassDefinition const& statement)
{
}

void Compiler::visit(InterfaceDefinition const& statement)
{
}

void Compiler::visit(AliasDefinition const& statement)
{
}

void Compiler::visit(FunctionDefinition const& statement)
{
	enter_scope();

	for (auto const& arg_name : statement.arguments)
	{
		define_variable(statement.name);
	}

	visit(&statement.block);

	auto instructions = leave_scope();
	int parameter_count = statement.arguments.size();

	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(statement.name, instructions, parameter_count));

	int constant_id = memory->get_constant_pool()->allocate(move(function_object));
	memory->get_code_section()->emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(GeneratorDefinition const& statement)
{
}

void Compiler::visit(FunctionMethodDefinition const& statement)
{
}

void Compiler::visit(GeneratorMethodDefinition const& statement)
{
}

void Compiler::visit(EnumDefinition const& statement)
{
}

void Compiler::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
	memory->get_code_section()->emit(OpCode::POP_FROM_STACK);
}

void Compiler::visit(Assert const& statement)
{
	visit(statement.expression);
	memory->get_code_section()->emit(OpCode::ASSERT);
}

void Compiler::visit(Implore const& statement)
{
	visit(statement.expression);
	memory->get_code_section()->emit(OpCode::IMPLORE);
}

void Compiler::visit(Swear const& statement)
{
	visit(statement.expression);
	memory->get_code_section()->emit(OpCode::SWEAR);
}

void Compiler::visit(Namespace const& statement)
{
}

void Compiler::visit(InfixOperatorDefinition const& statement)
{
}

void Compiler::visit(PrefixOperatorDefinition const& statement)
{
}

void Compiler::visit(PostfixOperatorDefinition const& statement)
{
}

// Expression

void Compiler::visit(const Expression_ptr expression)
{
	std::visit(overloaded{
		[&](int expr) { visit(expr); },
		[&](double expr) { visit(expr); },
		[&](std::wstring expr) { visit(expr); },
		[&](bool expr) { visit(expr); },
		[&](ListLiteral const& expr) { visit(expr); },
		[&](TupleLiteral const& expr) { visit(expr); },
		[&](MapLiteral const& expr) { visit(expr); },
		[&](SetLiteral const& expr) { visit(expr); },
		[&](NewObject const& expr) { visit(expr); },
		[&](TernaryCondition const& expr) { visit(expr); },
		[&](EnumMember const& expr) { visit(expr); },
		[&](Identifier const& expr) { visit(expr); },
		[&](Prefix const& expr) { visit(expr); },
		[&](Infix const& expr) { visit(expr); },
		[&](Postfix const& expr) { visit(expr); },
		[&](Call const& expr) { visit(expr); },
		[&](SpreadExpression const& expr) { visit(expr); },
		[&](TypePattern const& expr) { visit(expr); },
		[&](Assignment const& expr) { visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Statement before!");
		}
		}, *expression);
}

void Compiler::visit(std::vector<Expression_ptr> const& expressions)
{
	for (const auto expr : expressions)
	{
		visit(expr);
	}
}

void Compiler::visit(const int number)
{
	int constant_id = memory->get_constant_pool()->allocate(number);
	memory->get_code_section()->emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const double number)
{
	int constant_id = memory->get_constant_pool()->allocate(number);
	memory->get_code_section()->emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const std::wstring text)
{
	int constant_id = memory->get_constant_pool()->allocate(text);
	memory->get_code_section()->emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const bool boolean)
{
	if (boolean)
	{
		memory->get_code_section()->emit(OpCode::PUSH_CONSTANT_TRUE);
	}
	else
	{
		memory->get_code_section()->emit(OpCode::PUSH_CONSTANT_FALSE);
	}
}

void Compiler::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
	memory->get_code_section()->emit(OpCode::MAKE_LIST, expr.expressions.size());
}

void Compiler::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
	memory->get_code_section()->emit(OpCode::MAKE_TUPLE, expr.expressions.size());
}

void Compiler::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}

	memory->get_code_section()->emit(OpCode::MAKE_MAP, expr.pairs.size());
}

void Compiler::visit(SetLiteral const& expr)
{
	visit(expr.expressions);
	memory->get_code_section()->emit(OpCode::MAKE_SET, expr.expressions.size());
}

void Compiler::visit(NewObject const& expr)
{
}

void Compiler::visit(TernaryCondition const& expr)
{
}

void Compiler::visit(SpreadExpression const& expr)
{
}

void Compiler::visit(TypePattern const& expr)
{
}

void Compiler::visit(EnumMember const& expr)
{
}

void Compiler::visit(Identifier const& expr)
{
	auto scope = scope_stack.top();
	auto id = scope->symbol_table->lookup(expr.name);
	memory->get_code_section()->emit(OpCode::LOAD_LOCAL, id);
}

void Compiler::visit(Call const& expr)
{
	wstring function_name = expr.name;

	int constant_id = memory->get_constant_pool()->allocate(function_name);
	visit(expr.arguments);

	int argument_count = expr.arguments.size();
	memory->get_code_section()->emit(OpCode::CALL_FUNCTION, constant_id, argument_count);
}

void Compiler::visit(Prefix const& expr)
{
	visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::BANG:
	{
		memory->get_code_section()->emit(OpCode::UNARY_NOT);
		break;
	}
	case WTokenType::MINUS:
	{
		memory->get_code_section()->emit(OpCode::UNARY_NEGATIVE);
		break;
	}
	case WTokenType::PLUS:
	{
		memory->get_code_section()->emit(OpCode::UNARY_POSITIVE);
		break;
	}
	default: {
		break;
	}
	}
}

void Compiler::visit(Infix const& expr)
{
	visit(expr.right);
	visit(expr.left);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	{
		memory->get_code_section()->emit(OpCode::ADD);
		break;
	}
	case WTokenType::MINUS:
	{
		memory->get_code_section()->emit(OpCode::SUBTRACT);
		break;
	}
	case WTokenType::STAR:
	{
		memory->get_code_section()->emit(OpCode::MULTIPLY);
		break;
	}
	case WTokenType::DIVISION:
	{
		memory->get_code_section()->emit(OpCode::DIVISION);
		break;
	}
	case WTokenType::REMINDER:
	{
		memory->get_code_section()->emit(OpCode::REMINDER);
		break;
	}
	case WTokenType::POWER:
	{
		memory->get_code_section()->emit(OpCode::POWER);
		break;
	}
	case WTokenType::EQUAL_EQUAL:
	{
		memory->get_code_section()->emit(OpCode::EQUAL);
		break;
	}
	case WTokenType::BANG_EQUAL:
	{
		memory->get_code_section()->emit(OpCode::NOT_EQUAL);
		break;
	}
	case WTokenType::LESSER_THAN:
	{
		memory->get_code_section()->emit(OpCode::LESSER_THAN);
		break;
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		memory->get_code_section()->emit(OpCode::LESSER_THAN_EQUAL);
		break;
	}
	case WTokenType::GREATER_THAN:
	{
		memory->get_code_section()->emit(OpCode::GREATER_THAN);
		break;
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		memory->get_code_section()->emit(OpCode::GREATER_THAN_EQUAL);
		break;
	}
	case WTokenType::AND:
	{
		memory->get_code_section()->emit(OpCode::AND);
		break;
	}
	case WTokenType::OR:
	{
		memory->get_code_section()->emit(OpCode::OR);
		break;
	}
	default:
	{
		break;
	}
	}
}

void Compiler::visit(Postfix const& expr)
{
}

// Scope

CScope_ptr Compiler::enter_scope()
{
	if (scope_stack.size() > 0)
	{
		auto scope = scope_stack.top();
		auto enclosing_symbol_table = scope->symbol_table;
		scope_stack.push(make_shared<CScope>(enclosing_symbol_table));
	}
	else
	{
		scope_stack.push(make_shared<CScope>());
	}

	return scope_stack.top();
}

ByteVector Compiler::leave_scope()
{
	auto old_outer_scope = scope_stack.top();
	scope_stack.pop();

	ByteVector instructions = move(old_outer_scope->instructions);

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

// Utils

int Compiler::define_variable(wstring name)
{
	int id = memory->get_constant_pool()->allocate(name);

	auto scope = scope_stack.top();
	scope->symbol_table->define(name, id);

	return id;
}

int Compiler::create_label()
{
	int label = next_label++;
	return label;
}