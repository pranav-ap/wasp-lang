#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "SymbolScope.h"
#include "Statement.h"
#include "Expression.h"
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

void SemanticAnalyzer::execute(const File_ptr ast)
{
	current_scope = std::make_shared<SymbolScope>();
	type_system = std::make_shared<TypeSystem>();

	enter_scope(ScopeType::FILE);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	leave_scope();
}

// Statement

void SemanticAnalyzer::visit(const Statement_ptr statement)
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
		[&](Module const& stat) { visit(stat); },
		[&](InfixOperatorDefinition const& stat) { visit(stat); },
		[&](PrefixOperatorDefinition const& stat) { visit(stat); },
		[&](PostfixOperatorDefinition const& stat) { visit(stat); },

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

void SemanticAnalyzer::visit(IfBranch const& statement)
{
	Type_ptr condition_type = visit(statement.test);
	ASSERT(type_system->is_boolean_type(condition_type), "Boolean operand is expected");

	enter_scope(ScopeType::CONDITIONAL);
	visit(statement.body);
	leave_scope();

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value());
	}
}

void SemanticAnalyzer::visit(ElseBranch const& statement)
{
}

void SemanticAnalyzer::visit(WhileLoop const& statement)
{
	Type_ptr condition_type = visit(statement.expression);
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

void SemanticAnalyzer::visit(ClassDefinition const& statement)
{
}

void SemanticAnalyzer::visit(InterfaceDefinition const& statement)
{
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

void SemanticAnalyzer::visit(FunctionMethodDefinition const& statement)
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

void SemanticAnalyzer::visit(GeneratorMethodDefinition const& statement)
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

void SemanticAnalyzer::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
}

void SemanticAnalyzer::visit(Assert const& statement)
{
	Type_ptr type = visit(statement.expression);
	ASSERT(type_system->is_boolean_type(type), "Boolean operand is expected");
}

void SemanticAnalyzer::visit(Implore const& statement)
{
}

void SemanticAnalyzer::visit(Swear const& statement)
{
}

void SemanticAnalyzer::visit(Module const& statement)
{
}

void SemanticAnalyzer::visit(InfixOperatorDefinition const& statement)
{
}

void SemanticAnalyzer::visit(PrefixOperatorDefinition const& statement)
{
}

void SemanticAnalyzer::visit(PostfixOperatorDefinition const& statement)
{
}

// Expression

Type_ptr SemanticAnalyzer::visit(const Expression_ptr expression)
{
	return std::visit(overloaded{
		[&](double expr) { return visit(expr); },
		[&](std::wstring expr) { return visit(expr); },
		[&](bool expr) { return visit(expr); },
		[&](ListLiteral const& expr) { return visit(expr); },
		[&](TupleLiteral const& expr) {return  visit(expr); },
		[&](MapLiteral const& expr) { return visit(expr); },
		[&](SetLiteral const& expr) { return visit(expr); },
		[&](NewObject const& expr) { return visit(expr); },
		[&](TernaryCondition const& expr) { return visit(expr); },
		[&](EnumMember const& expr) { return visit(expr); },
		[&](Identifier const& expr) { return visit(expr); },
		[&](Prefix const& expr) { return visit(expr); },
		[&](Infix const& expr) { return visit(expr); },
		[&](Postfix const& expr) { return visit(expr); },
		[&](Call const& expr) { return visit(expr); },
		[&](SpreadExpression const& expr) { return visit(expr); },
		[&](TypePattern const& expr) { return visit(expr); },
		[&](Assignment const& expr) { return visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Statement before!");
			return MAKE_TYPE(NoneType());
		}
		}, *expression);
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

Type_ptr SemanticAnalyzer::visit(SetLiteral const& expr)
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

Type_ptr SemanticAnalyzer::visit(Assignment const& expr)
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
	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "Type mismatch in assignment");
}

Type_ptr SemanticAnalyzer::visit(TernaryCondition const& expr)
{
	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(SpreadExpression const& expr)
{
	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(TypePattern const& expr)
{
	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(NewObject const& expr)
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
	// does function with name exit

	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<CallableSymbol>(*symbol.value()), "This is not a Callable!");

	auto callable_symbol = get_if<CallableSymbol>(&*symbol.value());

	// infer types from arguments

	TypeVector actual_argument_types;

	for (auto const& argument : expr.arguments)
	{
		Type_ptr argument_type = visit(argument);
		actual_argument_types.push_back(argument_type);
	}

	// compare to expected

	std::optional<Type_ptr> return_type = std::visit(overloaded{
		[&](FunctionType const& type) -> std::optional<Type_ptr>
		{
			ASSERT(type_system->equal(current_scope, actual_argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},
		[&](GeneratorType const& type) -> std::optional<Type_ptr>
		{
			ASSERT(type_system->equal(current_scope, actual_argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},

		[](auto) -> std::optional<Type_ptr>
		{ return std::nullopt; }
		}, *callable_symbol->type);

	if (return_type.has_value())
	{
		return return_type.value();
	}

	return type_system->get_none_type();
}

Type_ptr SemanticAnalyzer::visit(Prefix const& expr)
{
	Type_ptr operand_type = visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	case WTokenType::UNARY_MINUS:
	{
		ASSERT(type_system->is_number_type(operand_type), "Number operand is expected");
		return type_system->get_number_type();
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

Type_ptr SemanticAnalyzer::visit(Postfix const& expr)
{
	Type_ptr operand_type = visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	case WTokenType::UNARY_MINUS:
	{
		ASSERT(type_system->is_number_type(operand_type), "Number operand is expected");
		return type_system->get_number_type();
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

Type_ptr SemanticAnalyzer::visit(Infix const& expr)
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
			ASSERT(type_system->is_number_type(rhs_operand_type), "Number operand is expected");
			return type_system->get_number_type();
		}
		else if (type_system->is_string_type(lhs_operand_type))
		{
			ASSERT(
				type_system->is_number_type(rhs_operand_type) ||
				type_system->is_string_type(rhs_operand_type),
				"Number or string operand is expected");

			return type_system->get_string_type();
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
		return type_system->get_number_type();
	}
	case WTokenType::EQUAL_EQUAL:
	case WTokenType::BANG_EQUAL:
	{
		if (type_system->is_number_type(lhs_operand_type))
		{
			ASSERT(type_system->is_number_type(rhs_operand_type), "Number operand is expected");
			return type_system->get_boolean_type();
		}
		else if (type_system->is_string_type(lhs_operand_type))
		{
			ASSERT(type_system->is_string_type(rhs_operand_type), "String operand is expected");
			return type_system->get_boolean_type();
		}
		else if (type_system->is_boolean_type(lhs_operand_type))
		{
			ASSERT(type_system->is_boolean_type(rhs_operand_type), "String operand is expected");
			return type_system->get_boolean_type();
		}
		else
		{
			FATAL("Number or string or boolean operand is expected");
		}

		break;
	}
	default:
	{
		FATAL("What the hell is this Binary statement?");
		break;
	}
	}

	return MAKE_TYPE(NoneType());
}

Type_ptr SemanticAnalyzer::visit(Identifier const& expr)
{
	std::optional<Symbol_ptr> symbol = current_scope->lookup(expr.name);
	OPT_CHECK(symbol);

	return std::visit(overloaded{
		[&](VariableSymbol const& sym) { return sym.type; },
		[&](CallableSymbol const& sym) { return sym.type; },
		[&](EnumSymbol const& sym) { return sym.type; },
		[&](UDTSymbol const& sym) { return sym.type; },
		[&](AliasSymbol const& sym) { return sym.type; },

		[&](auto) { return type_system->get_none_type(); }
		}, *symbol.value());
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