#include "pch.h"
#include "SemanticAnalyzer.h"
#include "SymbolTable.h"
#include "AST.h"
#include "Statement.h"
#include "Assertion.h"
#include <variant>
#include <vector>
#include <memory>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define NAME_EXISTS_CHECK(x) ASSERT(x.has_value(), "This name is not declared")
#define MAKE_SYMBOL(x) std::make_shared<Symbol>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::holds_alternative;
using std::wstring;
using std::get;

void SemanticAnalyzer::enter_scope(std::wstring id)
{
	auto new_scope = std::make_shared<ScopedSymbolTable>(current_symbol_table);
	current_symbol_table = new_scope;
	table_of_tables.insert({ id, new_scope });
}

void SemanticAnalyzer::leave_scope()
{
	current_symbol_table = current_symbol_table->enclosing_scope.value();
}

void SemanticAnalyzer::init()
{
	current_symbol_table = std::make_shared<ScopedSymbolTable>();
	table_of_tables.insert({ L"global", current_symbol_table });
}

std::map<std::wstring, ScopedSymbolTable_ptr> SemanticAnalyzer::execute(AST_ptr ast)
{
	init();

	for (auto statement : ast->nodes)
	{
		visit(statement);
	}

	return table_of_tables;
}

// Statement

void SemanticAnalyzer::visit(Statement_ptr statement)
{
	std::visit(overloaded{
		[&](Assignment& stat) { visit(stat); },
		[&](Branching& stat) { visit(stat); },
		[&](WhileLoop& stat) { visit(stat); },
		[&](ForInLoop& stat) { visit(stat); },
		[&](Break& stat) { visit(stat); },
		[&](Continue& stat) { visit(stat); },
		[&](Pass& stat) { visit(stat); },
		[&](Return& stat) { visit(stat); },
		[&](YieldStatement& stat) { visit(stat); },
		[&](VariableDefinition& stat) { visit(stat); },
		[&](UDTDefinition& stat) { visit(stat); },
		[&](AliasDefinition& stat) { visit(stat); },
		[&](FunctionDefinition& stat) { visit(stat); },
		[&](GeneratorDefinition& stat) { visit(stat); },
		[&](EnumDefinition& stat) { visit(stat); },
		[&](ImportCustom& stat) { visit(stat); },
		[&](ImportInBuilt& stat) { visit(stat); },
		[&](ExpressionStatement& stat) { visit(stat); },
		[&](AssertStatement& stat) { visit(stat); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void SemanticAnalyzer::visit(std::vector<Statement_ptr> block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
}

void SemanticAnalyzer::visit(Assignment& statement)
{
	for (const auto expr : statement.lhs_expressions)
	{
		// check if valid lhs expressions
	}

	visit(statement.rhs_expressions);
}

void SemanticAnalyzer::visit(Branching& statement)
{
	for (const auto [condition, block] : statement.branches)
	{
		visit(condition);

		enter_scope(block.id);
		visit(block);
		leave_scope();
	}

	enter_scope(statement.id);
	visit(statement.else_block);
	leave_scope();
}

void SemanticAnalyzer::visit(WhileLoop& statement)
{
	visit(statement.condition);

	enter_scope(statement.id);
	visit(statement.block);
	leave_scope();
}

void SemanticAnalyzer::visit(ForInLoop& statement)
{
	enter_scope(statement.id);

	auto symbol = MAKE_SYMBOL(VariableSymbol(
		statement.item_name,
		false,
		true,
		statement.item_type
	));

	current_symbol_table->define(statement.item_name, symbol);

	visit(statement.block);

	leave_scope();
}

void SemanticAnalyzer::visit(Break& statement)
{
}

void SemanticAnalyzer::visit(Continue& statement)
{
}

void SemanticAnalyzer::visit(Pass& statement)
{
}

void SemanticAnalyzer::visit(Return& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(YieldStatement& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
	}
}

void SemanticAnalyzer::visit(VariableDefinition& statement)
{
	auto symbol = MAKE_SYMBOL(VariableSymbol(
		statement.name,
		statement.is_public,
		statement.is_mutable,
		statement.type
	));

	current_symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(UDTDefinition& statement)
{
	auto symbol = MAKE_SYMBOL(UDTSymbol(
		statement.name,
		statement.is_public,
		statement.member_types,
		statement.is_public_member
	));

	current_symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(AliasDefinition& statement)
{
	auto symbol = MAKE_SYMBOL(AliasSymbol(
		statement.name,
		statement.is_public,
		statement.type
	));

	current_symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(FunctionDefinition& statement)
{
	auto symbol = MAKE_SYMBOL(FunctionSymbol(
		statement.name,
		statement.is_public,
		statement.arguments,
		statement.return_type,
		statement.body
	));

	current_symbol_table->define(statement.name, symbol);

	enter_scope(statement.id);
	visit(statement.body);
	leave_scope();
}

void SemanticAnalyzer::visit(GeneratorDefinition& statement)
{
}

void SemanticAnalyzer::visit(EnumDefinition& statement)
{
	auto symbol = MAKE_SYMBOL(EnumSymbol(statement.name, statement.is_public, statement.members));
	current_symbol_table->define(statement.name, symbol);
}

void SemanticAnalyzer::visit(ImportCustom& statement)
{
}

void SemanticAnalyzer::visit(ImportInBuilt& statement)
{
}

void SemanticAnalyzer::visit(ExpressionStatement& statement)
{
	visit(statement.expression);
}

void SemanticAnalyzer::visit(AssertStatement& statement)
{
	visit(statement.expression);
}

// Expression

void SemanticAnalyzer::visit(Expression_ptr expression)
{
	std::visit(overloaded{
		[&](double expr) { visit(expr); },
		[&](std::wstring expr) { visit(expr); },
		[&](bool expr) { visit(expr); },
		[&](ListLiteral& expr) { visit(expr); },
		[&](TupleLiteral& expr) { visit(expr); },
		[&](MapLiteral& expr) { visit(expr); },
		[&](UDTConstruct& expr) { visit(expr); },
		[&](UDTMemberAccess& expr) { visit(expr); },
		[&](EnumMember& expr) { visit(expr); },
		[&](Identifier& expr) { visit(expr); },
		[&](Call& expr) { visit(expr); },
		[&](Unary& expr) { visit(expr); },
		[&](Binary& expr) { visit(expr); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *expression);
}

void SemanticAnalyzer::visit(std::vector<Expression_ptr> expression_list)
{
	for (const auto expr : expression_list)
	{
		visit(expr);
	}
}

void SemanticAnalyzer::visit(double expr)
{
}

void SemanticAnalyzer::visit(std::wstring expr)
{
}

void SemanticAnalyzer::visit(bool expr)
{
}

void SemanticAnalyzer::visit(ListLiteral& expr)
{
	visit(expr.expressions);
}

void SemanticAnalyzer::visit(TupleLiteral& expr)
{
	visit(expr.expressions);
}

void SemanticAnalyzer::visit(MapLiteral& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}
}

void SemanticAnalyzer::visit(UDTConstruct& expr)
{
	std::optional<Symbol_ptr> symbol = current_symbol_table->lookup(expr.UDT_name);
	NAME_EXISTS_CHECK(symbol);
	ASSERT(holds_alternative<UDTSymbol>(*symbol.value()), "This is not a UDT!");

	// type check

	//for (const auto expr : expr.expressions)
	//{
	//
	//}
}

void SemanticAnalyzer::visit(UDTMemberAccess& expr)
{
	// check if next term is child of prev

	/*for (const auto term : expr.chain)
	{
		visit(term);
	}*/
}

void SemanticAnalyzer::visit(EnumMember& expr)
{
	std::optional<Symbol_ptr> symbol = current_symbol_table->lookup(expr.enum_name);
	NAME_EXISTS_CHECK(symbol);
	ASSERT(holds_alternative<EnumSymbol>(*symbol.value()), "This is not a Enum!");

	auto enum_symbol = get<EnumSymbol>(*symbol.value());
	auto enum_members = enum_symbol.members;

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

void SemanticAnalyzer::visit(Identifier& expr)
{
	std::optional<Symbol_ptr> symbol = current_symbol_table->lookup(expr.name);
	NAME_EXISTS_CHECK(symbol);
}

void SemanticAnalyzer::visit(Call& expr)
{
	std::optional<Symbol_ptr> symbol = current_symbol_table->lookup(expr.name);
	NAME_EXISTS_CHECK(symbol);
	ASSERT(holds_alternative<FunctionSymbol>(*symbol.value()), "This is not a function!");

	auto function_symbol = get<FunctionSymbol>(*symbol.value());

	// Argument type checking

	/*for (const auto arg : expr.arguments)
	{
		visit(arg);
	}*/
}

void SemanticAnalyzer::visit(Unary& expr)
{
	visit(expr.operand);
}

void SemanticAnalyzer::visit(Binary& expr)
{
	visit(expr.left);
	visit(expr.right);
}