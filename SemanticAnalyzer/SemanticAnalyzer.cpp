#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "SymbolScope.h"
#include "Statement.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(x) std::make_shared<Symbol>(x)
#define MAKE_TYPE(x) std::make_shared<Type>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::make_shared;
using std::move;

void SemanticAnalyzer::execute(const Module_ptr module_ast)
{
	current_scope = std::make_shared<SymbolScope>();
	type_system = std::make_shared<TypeSystem>();

	enter_scope(ScopeType::FILE);

	for (auto statement : module_ast->statements)
	{
		visit(statement);
	}

	leave_scope();
}

// Statement

void SemanticAnalyzer::visit(const Statement_ptr statement)
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

void SemanticAnalyzer::visit(std::vector<Statement_ptr> const& block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
}

void SemanticAnalyzer::visit(Assignment const& statement)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(statement.name);
	OPT_CHECK(symbol);

	Type_ptr lhs_type = std::visit(overloaded{
		[&](VariableSymbol const& sym) { return sym.type; },
		[&](CallableSymbol const& sym) { return sym.type; },
		[&](EnumSymbol const& sym) { return sym.type; },
		[&](UDTSymbol const& sym) { return sym.type; },
		[&](AliasSymbol const& sym) { return sym.type; },

		[](auto)
		{
			FATAL("Never Seen this Statement before!");
			return MAKE_TYPE(NoneType());
		}
		}, *symbol.value());

	Type_ptr rhs_type = visit(statement.expression);
	ASSERT(type_system->assignable(lhs_type, rhs_type), "Type mismatch in assignment");
}

void SemanticAnalyzer::visit(Branching const& statement)
{
	for (const auto branch : statement.branches)
	{
		auto condition = branch.first;
		Type_ptr condition_type = visit(condition);
		ASSERT(type_system->is_boolean_type(condition_type), "Boolean operand is expected");

		auto body = branch.second;

		enter_scope(ScopeType::CONDITIONAL);
		visit(body);
		leave_scope();
	}

	auto else_body = statement.else_block;

	enter_scope(ScopeType::CONDITIONAL);
	visit(else_body);
	leave_scope();
}

void SemanticAnalyzer::visit(WhileLoop const& statement)
{
	Type_ptr condition_type = visit(statement.condition);
	ASSERT(type_system->is_boolean_type(condition_type), "Boolean operand is expected");

	enter_scope(ScopeType::LOOP);
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop const& statement)
{
	enter_scope(ScopeType::LOOP);

	auto symbol = MAKE_SYMBOL(VariableSymbol(
		statement.item_name,
		false,
		true,
		statement.item_type
	));

	current_scope->define(statement.item_name, symbol);

	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(Break const& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Break is not expected in this block");
}

void SemanticAnalyzer::visit(Continue const& statement)
{
	ASSERT(current_scope->enclosed_in(ScopeType::LOOP), "Continue is not expected in this block");
}

void SemanticAnalyzer::visit(Pass const& statement)
{
	ASSERT(current_scope->enclosed_in({
		ScopeType::CONDITIONAL,
		ScopeType::FUNCTION,
		ScopeType::GENERATOR,
		ScopeType::LOOP,
		}), "Pass is not expected in this block");
}

void SemanticAnalyzer::visit(Return const& statement)
{
	ASSERT(current_scope->enclosed_in({
		ScopeType::FUNCTION,
		ScopeType::GENERATOR
		}), "Return is not expected in this block");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(YieldStatement const& statement)
{
	ASSERT(current_scope->enclosed_in({
		   ScopeType::FUNCTION,
		   ScopeType::GENERATOR
		}), "Yield is not expected in this block");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(VariableDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(VariableSymbol(
		statement.name,
		statement.is_public,
		statement.is_mutable,
		statement.type
	));

	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(UDTDefinition const& statement)
{
	auto type = MAKE_TYPE(UDTType(statement.name));

	auto symbol = MAKE_SYMBOL(UDTSymbol(
		statement.name,
		statement.is_public,
		statement.member_types,
		statement.is_public_member,
		type
	));

	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(AliasDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(AliasSymbol(
		statement.name,
		statement.is_public,
		statement.type
	));

	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(FunctionDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(CallableSymbol(
		statement.name,
		statement.is_public,
		statement.arguments,
		statement.type
	));

	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::FUNCTION);
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(GeneratorDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(CallableSymbol(
		statement.name,
		statement.is_public,
		statement.arguments,
		statement.type
	));

	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::GENERATOR);
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(EnumDefinition const& statement)
{
	auto type = MAKE_TYPE(EnumType(statement.name));

	auto symbol = MAKE_SYMBOL(EnumSymbol(
		statement.name,
		statement.is_public,
		statement.members,
		type
	));

	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(ImportCustom const& statement)
{
}

void SemanticAnalyzer::visit(ImportInBuilt const& statement)
{
}

void SemanticAnalyzer::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
}

void SemanticAnalyzer::visit(AssertStatement const& statement)
{
	Type_ptr type = visit(statement.expression);
	ASSERT(type_system->is_boolean_type(type), "Boolean operand is expected");
}

// Expression

Type_ptr SemanticAnalyzer::visit(const Expression_ptr expression)
{
	/*
		return std::visit(overloaded{
			[&](double expr) { return visit(expr); },
			[&](std::wstring expr) { return visit(expr); },
			[&](bool expr) { return visit(expr); },
			[&](ListLiteral const& expr) { return visit(expr); },
			[&](TupleLiteral const& expr) {return  visit(expr); },
			[&](MapLiteral const& expr) { return visit(expr); },
			[&](UDTConstruct const& expr) { return visit(expr); },
			[&](UDTMemberAccess const& expr) { return visit(expr); },
			[&](EnumMember const& expr) { return visit(expr); },
			[&](Identifier const& expr) { return visit(expr); },
			[&](Call const& expr) { return visit(expr); },
			[&](Unary const& expr) { return visit(expr); },
			[&](Binary const& expr) { return visit(expr); },

			[](auto)
			{
				FATAL("Never Seen this Statement before!");
				return MAKE_TYPE(NoneType());
			}
			}, *expression);
	*/

	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(const double expr)
{
	return type_system->get_number_type();
}

Type_ptr SemanticAnalyzer::visit(const std::wstring expr)
{
	return type_system->get_string_type();
}

Type_ptr SemanticAnalyzer::visit(const bool expr)
{
	return type_system->get_boolean_type();
}

Type_ptr SemanticAnalyzer::visit(ListLiteral const& expr)
{
	TypeVector types;

	for (auto const term : expr.expressions)
	{
		Type_ptr term_type = visit(term);
		types.push_back(move(term_type));
	}

	if (types.size() == 1)
	{
		Type_ptr list_type = MAKE_TYPE(ListType(types.front()));
		return list_type;
	}

	Type_ptr variant_type = MAKE_TYPE(VariantType(types));
	Type_ptr list_type = MAKE_TYPE(ListType(variant_type));
	return list_type;
}

Type_ptr SemanticAnalyzer::visit(TupleLiteral const& expr)
{
	TypeVector types;

	for (auto const term : expr.expressions)
	{
		Type_ptr term_type = visit(term);
		types.push_back(move(term_type));
	}

	Type_ptr tuple_type = MAKE_TYPE(TupleType(types));
	return tuple_type;
}

Type_ptr SemanticAnalyzer::visit(MapLiteral const& expr)
{
	TypeVector key_types;
	TypeVector value_types;

	for (const auto [key, value] : expr.pairs)
	{
		Type_ptr key_type = visit(key);
		key_types.push_back(move(key_type));

		Type_ptr value_type = visit(value);
		value_types.push_back(move(value_type));
	}

	Type_ptr key_type = (key_types.size() == 1) ? key_types.front() : MAKE_TYPE(VariantType(key_types));
	Type_ptr value_type = (value_types.size() == 1) ? value_types.front() : MAKE_TYPE(VariantType(value_types));

	Type_ptr map_type = MAKE_TYPE(MapType(key_type, value_type));
	return map_type;
}

Type_ptr SemanticAnalyzer::visit(UDTConstruct const& expr)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.UDT_name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<UDTSymbol>(*symbol.value()), "This is not a UDT!");

	// must check args
	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(UDTMemberAccess const& expr)
{
	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(EnumMember const& expr)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.enum_name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<EnumSymbol>(*symbol.value()), "This is not a Enum!");

	auto enum_symbol = get_if<EnumSymbol>(&*symbol.value());
	auto enum_members = enum_symbol->members;

	wstring enum_string = expr.enum_name;

	for (const auto member : expr.member_chain)
	{
		enum_string.append(L"::");
		enum_string.append(member);
	}

	ASSERT(
		std::find(enum_members.begin(), enum_members.end(), enum_string) != enum_members.end(),
		"Enum does not contain this member"
	);

	return enum_symbol->type;
}

Type_ptr SemanticAnalyzer::visit(Call const& expr)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<CallableSymbol>(*symbol.value()), "This is not a Callable!");

	auto function_symbol = get_if<CallableSymbol>(&*symbol.value());

	TypeVector actual_argument_types;

	for (auto const& argument : expr.arguments)
	{
		Type_ptr argument_type = visit(argument);
		actual_argument_types.push_back(argument_type);
	}

	Type_ptr return_type = std::visit(overloaded{
		[&](FunctionType const& type)
		{
			ASSERT(type_system->equal(actual_argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},
		[&](GeneratorType const& type)
		{
			ASSERT(type_system->equal(actual_argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},

		[](auto) { return MAKE_TYPE(NoneType()); }
		}, *function_symbol->type);

	return return_type;
}

Type_ptr SemanticAnalyzer::visit(Unary const& expr)
{
	Type_ptr operand_type = visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	case WTokenType::UNARY_MINUS:
	{
		ASSERT(type_system->is_number_type(operand_type), "Number operand is expected");
		return MAKE_TYPE(NumberType());
	}
	case WTokenType::BANG:
	{
		ASSERT(type_system->is_boolean_type(operand_type), "Boolean operand is expected");
		return MAKE_TYPE(BooleanType());
	}
	default:
	{
		FATAL("What the hell is this unary statement?");
		break;
	}
	}

	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(Binary const& expr)
{
	Type_ptr lhs_operand_type = visit(expr.left);
	Type_ptr rhs_operand_type = visit(expr.right);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	case WTokenType::STAR:
	case WTokenType::POWER:
	{
		if (type_system->is_number_type(lhs_operand_type))
		{
			ASSERT(type_system->is_number_type(rhs_operand_type), "String operand is expected");
			return MAKE_TYPE(NumberType());
		}
		else if (type_system->is_string_type(lhs_operand_type))
		{
			ASSERT(
				type_system->is_number_type(rhs_operand_type) ||
				type_system->is_string_type(rhs_operand_type),
				"Number or string operand is expected");

			return MAKE_TYPE(StringType());
		}
		else
		{
			FATAL("Number or string operand is expected");
		}

		break;
	}
	case WTokenType::MINUS:
	case WTokenType::DIVISION:
	case WTokenType::REMINDER:
	case WTokenType::LESSER_THAN:
	case WTokenType::LESSER_THAN_EQUAL:
	case WTokenType::GREATER_THAN:
	case WTokenType::GREATER_THAN_EQUAL:
	{
		ASSERT(type_system->is_number_type(lhs_operand_type), "Number operand is expected");
		ASSERT(type_system->is_number_type(rhs_operand_type), "Number operand is expected");
		return MAKE_TYPE(NumberType());
	}
	default:
	{
		FATAL("What the hell is this unary statement?");
		break;
	}
	}

	return MAKE_TYPE(NoneType());
}

Symbol_ptr SemanticAnalyzer::visit(Identifier const& expr)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.name);
	OPT_CHECK(symbol);
	return symbol.value();
}

// Utils

void SemanticAnalyzer::enter_scope(ScopeType scope_type)
{
	NULL_CHECK(current_scope);

	auto child_scope = std::make_shared<SymbolScope>(current_scope, scope_type);
	current_scope = child_scope;
}

void SemanticAnalyzer::leave_scope()
{
	NULL_CHECK(current_scope);
	OPT_CHECK(current_scope->enclosing_scope);
	current_scope = current_scope->enclosing_scope.value();
}