#pragma once
#include "pch.h"
#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Statement.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_SYMBOL(x) std::make_shared<Symbol>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get_if;

void SemanticAnalyzer::execute(const Module_ptr module_ast)
{
	symbol_table = std::make_shared<SymbolTable>();
	enter_scope();

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
	visit(statement.lhs_expressions);
	visit(statement.rhs_expressions);
}

void SemanticAnalyzer::visit(Branching const& statement)
{
	for (const auto branch : statement.branches)
	{
		auto condition = branch.first;
		visit(condition);

		auto body = branch.second;

		enter_scope();
		visit(body);
		leave_scope();
	}

	auto else_body = statement.else_block;

	enter_scope();
	visit(else_body);
	leave_scope();
}

void SemanticAnalyzer::visit(WhileLoop const& statement)
{
	visit(statement.condition);

	enter_scope();
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop const& statement)
{
	enter_scope();

	auto symbol = MAKE_SYMBOL(VariableSymbol(
		statement.item_name,
		false,
		true,
		statement.item_type
	));

	symbol_table->define(statement.item_name, symbol);

	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(Break const& statement)
{
}

void SemanticAnalyzer::visit(Continue const& statement)
{
}

void SemanticAnalyzer::visit(Pass const& statement)
{
}

void SemanticAnalyzer::visit(Return const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(YieldStatement const& statement)
{
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

	symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(UDTDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(UDTSymbol(
		statement.name,
		statement.is_public,
		statement.member_types,
		statement.is_public_member
	));

	symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(AliasDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(AliasSymbol(
		statement.name,
		statement.is_public,
		statement.type
	));

	symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(FunctionDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(FunctionSymbol(
		statement.name,
		statement.is_public,
		statement.arguments,
		statement.return_type
	));

	symbol_table->define(statement.name, symbol);

	enter_scope();
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(GeneratorDefinition const& statement)
{
}

void SemanticAnalyzer::visit(EnumDefinition const& statement)
{
	auto symbol = MAKE_SYMBOL(EnumSymbol(
		statement.name,
		statement.is_public,
		statement.members
	));

	symbol_table->define(statement.name, symbol);
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
	visit(statement.expression);
}

// Expression

void SemanticAnalyzer::visit(const Expression_ptr expression)
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

void SemanticAnalyzer::visit(std::vector<Expression_ptr> const& expressions)
{
	for (const auto expr : expressions)
	{
		visit(expr);
	}
}

void SemanticAnalyzer::visit(const double expr)
{
}

void SemanticAnalyzer::visit(const std::wstring expr)
{
}

void SemanticAnalyzer::visit(const bool expr)
{
}

void SemanticAnalyzer::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
}

void SemanticAnalyzer::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
}

void SemanticAnalyzer::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}
}

void SemanticAnalyzer::visit(UDTConstruct const& expr)
{
	std::optional<Symbol_ptr> symbol = symbol_table->lookup(expr.UDT_name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<UDTSymbol>(*symbol.value()), "This is not a UDT!");
}

void SemanticAnalyzer::visit(UDTMemberAccess const& expr)
{
}

void SemanticAnalyzer::visit(EnumMember const& expr)
{
	std::optional<Symbol_ptr> symbol = symbol_table->lookup(expr.enum_name);
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
}

void SemanticAnalyzer::visit(Identifier const& expr)
{
	std::optional<Symbol_ptr> symbol = symbol_table->lookup(expr.name);
	OPT_CHECK(symbol);
}

void SemanticAnalyzer::visit(Call const& expr)
{
	std::optional<Symbol_ptr> symbol = symbol_table->lookup(expr.name);
	OPT_CHECK(symbol);
	ASSERT(holds_alternative<FunctionSymbol>(*symbol.value()), "This is not a function!");

	auto function_symbol = get_if<FunctionSymbol>(&*symbol.value());
}

void SemanticAnalyzer::visit(Unary const& expr)
{
	visit(expr.operand);
}

void SemanticAnalyzer::visit(Binary const& expr)
{
	visit(expr.left);
	visit(expr.right);
}

// Utils

void SemanticAnalyzer::enter_scope()
{
	NULL_CHECK(symbol_table);

	auto child_table = std::make_shared<SymbolTable>(symbol_table);
	symbol_table = child_table;
}

void SemanticAnalyzer::leave_scope()
{
	NULL_CHECK(symbol_table);
	OPT_CHECK(symbol_table->enclosing_scope);
	symbol_table = symbol_table->enclosing_scope.value();
}