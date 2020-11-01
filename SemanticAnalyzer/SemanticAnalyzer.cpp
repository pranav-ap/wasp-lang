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
#define MAKE_SYMBOL(a, b, c, d) std::make_shared<Symbol>(a, b, c, d)
#define MAKE_TYPE(x) std::make_shared<Type>(x)

#define PUBLIC_SYMBOL true
#define PRIVATE_SYMBOL false
#define MUTABLE_SYMBOL true
#define CONST_SYMBOL false

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;
using std::vector;
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
		[&](FunctionDefinition const& stat) { visit(stat); },
		[&](GeneratorDefinition const& stat) { visit(stat); },
		[&](FunctionMemberDefinition const& stat) { visit(stat); },
		[&](GeneratorMemberDefinition const& stat) { visit(stat); },
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

void SemanticAnalyzer::visit(std::vector<Statement_ptr> const& block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
}

void SemanticAnalyzer::visit(IfBranch const& statement)
{
	enter_scope(ScopeType::CONDITIONAL);

	std::visit(overloaded{
		[&](Assignment const& expr)
		{
			Type_ptr right_type = visit(expr.rhs_expression);

			const auto [identifier, left_type] = deconstruct_type_pattern(expr.lhs_expression);
			ASSERT(type_system->assignable(current_scope, left_type, right_type), "Type mismatch in assignment");
			type_system->expect_condition_type(current_scope, left_type);

			auto symbol = MAKE_SYMBOL(identifier, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
			current_scope->define(identifier, symbol);
		},

		[&](auto)
		{
			Type_ptr condition_type = visit(statement.test);
			type_system->expect_condition_type(current_scope, condition_type);
		}
		}, *statement.test);

	visit(statement.body);
	leave_scope();

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value());
	}
}

void SemanticAnalyzer::visit(ElseBranch const& statement)
{
	enter_scope(ScopeType::CONDITIONAL);
	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(WhileLoop const& statement)
{
	Type_ptr condition_type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, condition_type);

	enter_scope(ScopeType::LOOP);
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop const& statement)
{
	Type_ptr right_type = visit(statement.rhs_expression);
	type_system->expect_iterable_type(current_scope, right_type);

	enter_scope(ScopeType::LOOP);

	const auto [identifier, left_type] = deconstruct_type_pattern(statement.lhs_expression);

	auto symbol = MAKE_SYMBOL(identifier, left_type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
	current_scope->define(identifier, symbol);

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
		ScopeType::GENERATOR,
		ScopeType::CLASS_FUNCTION,
		ScopeType::CLASS_GENERATOR
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
		   ScopeType::GENERATOR,
		   ScopeType::CLASS_FUNCTION,
		   ScopeType::CLASS_GENERATOR
		}), "Yield is not expected in this block");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(VariableDefinition const& statement)
{
	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an Identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	Type_ptr lhs_type = type_system->allocate(current_scope, statement.type);
	Type_ptr rhs_type = visit(statement.rhs_expression);

	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "Type mismatch in assignment");

	auto symbol = MAKE_SYMBOL(identifier->name, lhs_type, statement.is_public, statement.is_mutable);
	current_scope->define(identifier->name, symbol);
}

void SemanticAnalyzer::visit(ClassDefinition const& statement)
{
	Type_ptr type = type_system->allocate(current_scope, MAKE_TYPE(ClassType(
		statement.name,
		statement.interfaces,
		statement.base_types,
		statement.member_types,
		statement.is_public_member
	)));

	auto symbol = MAKE_SYMBOL(statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(InterfaceDefinition const& statement)
{
	Type_ptr type = type_system->allocate(current_scope, MAKE_TYPE(InterfaceType(
		statement.name,
		statement.interfaces,
		statement.base_types,
		statement.member_types,
		statement.is_public_member
	)));

	auto symbol = MAKE_SYMBOL(statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(AliasDefinition const& statement)
{
	Type_ptr type = type_system->allocate(current_scope, MAKE_TYPE(AliasType(statement.name, statement.type)));
	auto symbol = MAKE_SYMBOL(statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(FunctionDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(statement.name, statement.type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::FUNCTION);

	auto function_type = type_system->extract_function_type(statement.type);
	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Type_ptr type = function_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(GeneratorDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(statement.name, statement.type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);

	enter_scope(ScopeType::GENERATOR);

	auto generator_type = type_system->extract_generator_type(statement.type);
	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Type_ptr type = generator_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(FunctionMemberDefinition const& statement)
{
	enter_scope(ScopeType::CLASS_FUNCTION);

	auto function_method_type = type_system->extract_function_member_type(statement.type);
	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Type_ptr type = function_method_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();

	Symbol_ptr class_symbol = current_scope->lookup(statement.type_name);
	NULL_CHECK(class_symbol);
	auto class_type = type_system->extract_class_type(class_symbol->type);

	ASSERT(class_type->members.contains(statement.name), "Member does not exist");
	class_type->members.at(statement.name) = statement.type;
}

void SemanticAnalyzer::visit(GeneratorMemberDefinition const& statement)
{
	enter_scope(ScopeType::CLASS_GENERATOR);

	auto generator_method_type = type_system->extract_generator_member_type(statement.type);
	int arg_index = 0;

	for (auto const arg_name : statement.arguments)
	{
		Type_ptr type = generator_method_type->input_types.at(arg_index);
		auto symbol = MAKE_SYMBOL(arg_name, type, PRIVATE_SYMBOL, MUTABLE_SYMBOL);
		current_scope->define(arg_name, symbol);

		arg_index++;
	}

	visit(statement.body);
	leave_scope();

	Symbol_ptr class_symbol = current_scope->lookup(statement.type_name);
	NULL_CHECK(class_symbol);
	auto class_type = type_system->extract_class_type(class_symbol->type);

	ASSERT(class_type->members.contains(statement.name), "Member does not exist");
	class_type->members.at(statement.name) = statement.type;
}

void SemanticAnalyzer::visit(EnumDefinition const& statement)
{
	auto type = MAKE_TYPE(EnumType(statement.name, statement.members));
	auto symbol = MAKE_SYMBOL(statement.name, type, statement.is_public, CONST_SYMBOL);
	current_scope->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(ExpressionStatement const& statement)
{
	Type_ptr type = visit(statement.expression);
}

void SemanticAnalyzer::visit(Assert const& statement)
{
	Type_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Implore const& statement)
{
	Type_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Swear const& statement)
{
	Type_ptr type = visit(statement.expression);
	type_system->expect_condition_type(current_scope, type);
}

void SemanticAnalyzer::visit(Namespace const& statement)
{
	enter_scope(ScopeType::NAMESPACE);
	visit(statement.statements);
	leave_scope();
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
	Type_ptr type = std::visit(overloaded{
		[&](int expr) { return visit(expr); },
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
		[&](MemberAccess const& expr) { return visit(expr); },
		[&](Identifier const& expr) { return visit(expr); },
		[&](Prefix const& expr) { return visit(expr); },
		[&](Infix const& expr) { return visit(expr); },
		[&](Postfix const& expr) { return visit(expr); },
		[&](Call const& expr) { return visit(expr); },
		[&](TypePattern const& expr) { return visit(expr); },
		[&](Assignment const& expr) { return visit(expr); },
		[&](Spread const& expr) { return visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Statement before!");
			return MAKE_TYPE(NoneType());
		}
		}, *expression);

	return type_system->allocate(current_scope, type);
}

TypeVector SemanticAnalyzer::visit(ExpressionVector const& expressions)
{
	TypeVector types;

	for (auto const& argument : expressions)
	{
		Type_ptr type = visit(argument);
		types.push_back(type);
	}

	return types;
}

Type_ptr SemanticAnalyzer::visit(const int expr)
{
	return type_system->get_int_type();
}

Type_ptr SemanticAnalyzer::visit(const double expr)
{
	return type_system->get_float_type();
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
	TypeVector types = visit(expr.expressions);

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
	TypeVector types = visit(expr.expressions);

	Type_ptr tuple_type = MAKE_TYPE(TupleType(types));
	return tuple_type;
}

Type_ptr SemanticAnalyzer::visit(SetLiteral const& expr)
{
	TypeVector types = visit(expr.expressions);

	Type_ptr set_type = MAKE_TYPE(SetType(types));
	return set_type;
}

Type_ptr SemanticAnalyzer::visit(MapLiteral const& expr)
{
	TypeVector key_types;
	TypeVector value_types;

	for (const auto [key, value] : expr.pairs)
	{
		Type_ptr key_type = visit(key);
		type_system->expect_key_type(current_scope, key_type);
		key_types.push_back(move(key_type));

		Type_ptr value_type = visit(value);
		value_types.push_back(move(value_type));
	}

	Type_ptr key_type = (key_types.size() == 1) ? key_types.front() : MAKE_TYPE(VariantType(key_types));
	Type_ptr value_type = (value_types.size() == 1) ? value_types.front() : MAKE_TYPE(VariantType(value_types));

	Type_ptr map_type = MAKE_TYPE(MapType(key_type, value_type));
	return map_type;
}

Type_ptr SemanticAnalyzer::visit(Assignment const& expression)
{
	Type_ptr lhs_type = visit(expression.lhs_expression);
	Type_ptr rhs_type = visit(expression.rhs_expression);

	ASSERT(type_system->assignable(current_scope, lhs_type, rhs_type), "Type mismatch in assignment");

	return rhs_type;
}

Type_ptr SemanticAnalyzer::visit(TernaryCondition const& expression)
{
	Type_ptr condition_type = visit(expression.condition);
	type_system->expect_condition_type(current_scope, condition_type);

	Type_ptr true_type = visit(expression.true_expression);
	Type_ptr false_type = visit(expression.false_expression);

	if (type_system->equal(current_scope, true_type, false_type))
	{
		return true_type;
	}

	return MAKE_TYPE(VariantType({ true_type, false_type }));
}

Type_ptr SemanticAnalyzer::visit(Spread const& expr)
{
	Type_ptr operand_type = visit(expr.expression);
	type_system->expect_spreadable_type(current_scope, operand_type);
	return operand_type;
}

Type_ptr SemanticAnalyzer::visit(TypePattern const& expr)
{
	FATAL("TypePattern must be handled by parent nodes");
	return type_system->get_none_type();
}

Type_ptr SemanticAnalyzer::visit(NewObject const& expr)
{
	TypeVector types = visit(expr.expressions);

	auto symbol = current_scope->lookup(expr.type_name);
	NULL_CHECK(symbol);

	auto class_type = type_system->extract_class_type(symbol->type);

	wstring constructor_name = L"constructor";
	auto exists = class_type->members.contains(constructor_name);
	ASSERT(exists, "Required constructor must exist");

	return symbol->type;
}

Type_ptr SemanticAnalyzer::visit(EnumMember const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.member_chain.front());
	NULL_CHECK(symbol);

	auto enum_type = type_system->extract_enum_type(symbol->type);
	wstring enum_string = concat(expr.member_chain, L"::");
	ASSERT(enum_type->members.contains(enum_string), "Enum does not contain this member");

	return symbol->type;
}

Type_ptr SemanticAnalyzer::visit(Call const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	NULL_CHECK(symbol);

	TypeVector argument_types = visit(expr.arguments);

	Type_ptr return_type = std::visit(overloaded{
		[&](FunctionType const& type)
		{
			ASSERT(type_system->equal(current_scope, argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},
		[&](GeneratorType const& type)
		{
			ASSERT(type_system->equal(current_scope, argument_types, type.input_types), "Argument mismatch in call");
			return type.return_type;
		},
		[&](auto)
		{
			return type_system->get_none_type();
		}
		}, *symbol->type);

	return return_type;
}

Type_ptr SemanticAnalyzer::visit(Prefix const& expr)
{
	Type_ptr operand_type = visit(expr.operand);

	Type_ptr result_type = type_system->infer_type(current_scope, operand_type, expr.op->type);
	return result_type;
}

Type_ptr SemanticAnalyzer::visit(Postfix const& expr)
{
	Type_ptr operand_type = visit(expr.operand);

	return type_system->get_none_type();
}

Type_ptr SemanticAnalyzer::visit(Infix const& expr)
{
	Type_ptr lhs_operand_type = visit(expr.left);
	Type_ptr rhs_operand_type = visit(expr.right);

	Type_ptr result_type = type_system->infer_type(current_scope, lhs_operand_type, expr.op->type, rhs_operand_type);
	return result_type;
}

Type_ptr SemanticAnalyzer::visit(Identifier const& expr)
{
	Symbol_ptr symbol = current_scope->lookup(expr.name);
	NULL_CHECK(symbol);
	return symbol->type;
}

Type_ptr SemanticAnalyzer::visit(MemberAccess const& expr)
{
	Type_ptr lhs_operand_type = visit(expr.left);
	auto lhs_class_type = type_system->extract_class_type(lhs_operand_type);

	return std::visit(overloaded{
		[&](Identifier const& identifier)
		{
			ASSERT(lhs_class_type->members.contains(identifier.name), "Identifier is not found in class");
			Type_ptr rhs_type = lhs_class_type->members.at(identifier.name);
			return rhs_type;
		},

		[&](Call const& call)
		{
			ASSERT(lhs_class_type->members.contains(call.name), "Call is not found in class");
			Type_ptr rhs_type = lhs_class_type->members.at(call.name);

			return std::visit(overloaded{
				[&](FunctionType const& type) { return type.return_type; },
				[&](GeneratorType const& type) { return type.return_type; },
				[&](FunctionMemberType const& type) { return type.return_type; },
				[&](GeneratorMemberType const& type) { return type.return_type; },
				[&](auto)
				{
					FATAL("Expected a FunctionType or GeneratorType");
					return type_system->get_none_type();
				}
				}, *rhs_type);
		},

		[&](auto)
		{
			FATAL("MemberAccess is invalid!");
			return type_system->get_none_type();
		}
		}, *expr.right);
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

wstring SemanticAnalyzer::concat(StringVector items, wstring middle)
{
	wstring final_string = L"";

	for (const auto member : items)
	{
		final_string.append(middle);
		final_string.append(member);
	}

	final_string = final_string.substr(2, final_string.size());
	return final_string;
}

std::tuple<std::wstring, Type_ptr> SemanticAnalyzer::deconstruct_type_pattern(Expression_ptr expression)
{
	ASSERT(holds_alternative<TypePattern>(*expression), "Expected a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*expression);

	Type_ptr type = type_system->allocate(current_scope, type_pattern->type);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Expected an Identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	return std::make_tuple(identifier->name, type);
}