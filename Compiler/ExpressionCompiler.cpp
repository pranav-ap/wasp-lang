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
		[&](TernaryCondition const& expr) { visit(expr); },
		[&](Identifier const& expr) { visit(expr); },
		[&](Prefix const& expr) { visit(expr); },
		[&](Infix const& expr) { visit(expr); },
		[&](Postfix const& expr) { visit(expr); },
		[&](TypePattern const& expr) { visit(expr); },
		[&](UntypedAssignment const& expr) { visit(expr); },
		[&](TypedAssignment const& expr) { visit(expr); },
		[&](Call const& expr) { visit(expr); },
		[&](EnumMember const& expr) { visit(expr); },
		[&](Spread const& expr) { visit(expr); },
		[&](TypeOf const& expr) { visit(expr); },
		[&](Is const& expr) { visit(expr); },
		[&](As const& expr) { visit(expr); },

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
	int id = constant_pool->allocate(number);
	emit(OpCode::PUSH_CONSTANT, id);
}

void Compiler::visit(const double number)
{
	int id = constant_pool->allocate(number);
	emit(OpCode::PUSH_CONSTANT, id);
}

void Compiler::visit(const std::wstring text)
{
	int id = constant_pool->allocate(text);
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

void Compiler::visit(SetLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_SET, expr.expressions.size());
}

void Compiler::visit(TernaryCondition const& expr)
{
	set_current_scope(expr.scope);
	visit(expr.condition);

	int	alternative_branch_label = create_label();
	int	exit_branch_label = create_label();

	emit(OpCode::POP_JUMP_IF_FALSE, alternative_branch_label);
	visit(expr.true_expression);
	emit(OpCode::POP_JUMP, exit_branch_label);

	emit(OpCode::LABEL, alternative_branch_label);
	visit(expr.false_expression);

	emit(OpCode::LABEL, exit_branch_label);

	leave_scope();
}

void Compiler::visit(TypePattern const& expr)
{
	FATAL("TypePattern cannot be visited");
}

void Compiler::visit(Identifier const& expr)
{
	int id = current_scope->lookup(expr.name)->id;
	id = get_pool_id(id);
	emit(OpCode::LOAD_LOCAL, id);
}

void Compiler::visit(Spread const& expr)
{
	if (expr.is_rvalue)
	{
	}
}

void Compiler::visit(TypeOf const& expr)
{
	visit(expr.expression);
	emit(OpCode::POP_FROM_STACK);

	auto symbol = current_scope->lookup(expr.name);
	int id = constant_pool->allocate_type(symbol->type);

	emit(OpCode::PUSH_CONSTANT, id);
}

void Compiler::visit(Is const& expr)
{
	visit(expr.left);
	emit(OpCode::POP_FROM_STACK);

	auto left_symbol = current_scope->lookup(expr.left_name);
	int id = constant_pool->allocate_type(left_symbol->type);
	emit(OpCode::PUSH_CONSTANT, id);

	auto right_symbol = current_scope->lookup(expr.right_name);
	id = constant_pool->allocate_type(right_symbol->type);
	emit(OpCode::PUSH_CONSTANT, id);

	emit(OpCode::EQUAL);
}

void Compiler::visit(As const& expr)
{
	visit(expr.left);

	auto right_symbol = current_scope->lookup(expr.right_name);
	int id = constant_pool->allocate_type(right_symbol->type);
	emit(OpCode::PUSH_CONSTANT, id);

	emit(OpCode::CONVERT_TYPE);
}

void Compiler::visit(Prefix const& expr)
{
	visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::BANG:
	{
		emit(OpCode::UNARY_NOT);
		break;
	}
	case WTokenType::MINUS:
	{
		emit(OpCode::UNARY_NEGATIVE);
		break;
	}
	case WTokenType::TYPE_OF:
	case WTokenType::PLUS:
	{
		// do nothing
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
	case WTokenType::QUESTION_QUESTION:
	{
		emit(OpCode::NULLISH_COALESE);
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

void Compiler::visit(Postfix const& expr)
{
}

void Compiler::visit(UntypedAssignment const& statement)
{
	visit(statement.rhs_expression);

	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	int id = current_scope->lookup(identifier->name)->id;
	id = get_pool_id(id);

	emit(OpCode::STORE_LOCAL, id);
}

void Compiler::visit(TypedAssignment const& statement)
{
	FATAL("TypedAssignment must be handled by parent nodes");
}

void Compiler::visit(EnumMember const& expr)
{
	auto enum_name = expr.chain.front();
	auto enum_symbol = current_scope->lookup(enum_name);

	ASSERT(holds_alternative<EnumType>(*enum_symbol->type), "Expected Enum Type");
	auto enum_type = get_if<EnumType>(&*enum_symbol->type);
	int member_id = enum_type->members.at(expr.chain_str);

	int pool_id = constant_pool->allocate_enum_member(enum_symbol->id, member_id, expr.chain_str);
	emit(OpCode::PUSH_CONSTANT, pool_id);

	name_map[pool_id] = expr.chain_str;
}

void Compiler::visit(Call const& expr)
{
	visit(expr.arguments);

	int count = expr.arguments.size();

	int id = current_scope->lookup(expr.name)->id;
	id = get_pool_id(id);

	if (expr.is_builtin)
	{
		emit(OpCode::CALL_BUILTIN_FUN, id, count);
	}
	else
	{
		emit(OpCode::CALL_FUNCTION, id, count);
	}
}