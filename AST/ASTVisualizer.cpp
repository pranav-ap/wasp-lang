#pragma once
#include "pch.h"
#include "ASTVisualizer.h"
#include "Assertion.h"
#include <string>
#include <fstream>

#define ARROW L" -> "
#define NEW_LINE L" \n "
#define COLON L";"

using std::to_wstring;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void ASTVisualizer::save(int id, std::wstring label)
{
	std::wstring id_str = quote(to_wstring(id));
	label = quote(label);

	content.append(id_str + L"   [label=" + label + L"]" + COLON + NEW_LINE);
}

void ASTVisualizer::save(int child, int parent, std::wstring label)
{
	std::wstring child_str = quote(to_wstring(child));
	std::wstring parent_str = quote(to_wstring(parent));
	label = quote(label);

	content.append(child_str + L"   [label=" + label + L"]" + COLON + NEW_LINE);
	content.append(parent_str + ARROW + child_str + COLON + NEW_LINE);
}

std::wstring ASTVisualizer::quote(std::wstring part)
{
	return L"\"" + part + L"\"";
}

// Statement

void ASTVisualizer::visit(const Statement_ptr statement, int parent_id)
{
	std::visit(overloaded{
		[&](Branching const& stat) { visit(stat, parent_id); },
		[&](WhileLoop const& stat) { visit(stat, parent_id); },
		[&](ForInLoop const& stat) { visit(stat, parent_id); },
		[&](Break const& stat) { visit(stat, parent_id); },
		[&](Continue const& stat) { visit(stat, parent_id); },
		[&](Return const& stat) { visit(stat, parent_id); },
		[&](YieldStatement const& stat) { visit(stat, parent_id); },
		[&](VariableDefinition const& stat) { visit(stat, parent_id); },
		[&](UDTDefinition const& stat) { visit(stat, parent_id); },
		[&](AliasDefinition const& stat) { visit(stat, parent_id); },
		[&](FunctionDefinition const& stat) { visit(stat, parent_id); },
		[&](GeneratorDefinition const& stat) { visit(stat, parent_id); },
		[&](EnumDefinition const& stat) { visit(stat, parent_id); },
		[&](ExpressionStatement const& stat) { visit(stat, parent_id); },
		[&](AssertStatement const& stat) { visit(stat, parent_id); },
		[&](ImploreStatement const& stat) { visit(stat, parent_id); },
		[&](SwearStatement const& stat) { visit(stat, parent_id); },

		[](auto) { FATAL("Never Seen this Statement before! So I cannot print it!"); }
		}, *statement);
}

void ASTVisualizer::visit(std::vector<Statement_ptr> const& statements, int parent_id)
{
	for (const auto statement : statements)
	{
		visit(statement, parent_id);
	}
}

void ASTVisualizer::visit(Branching const& statement, int parent_id)
{
	for (const auto [condition, block] : statement.branches)
	{
	}
}

void ASTVisualizer::visit(WhileLoop const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"while");

	visit(statement.condition, id);
	visit(statement.block, id);
}

void ASTVisualizer::visit(ForInLoop const& statement, int parent_id)
{
}

void ASTVisualizer::visit(Break const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"break");
}

void ASTVisualizer::visit(Continue const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"continue");
}

void ASTVisualizer::visit(Return const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"return");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value(), id);
	}
}

void ASTVisualizer::visit(YieldStatement const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"yield");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value(), id);
	}
}

void ASTVisualizer::visit(VariableDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(UDTDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(AliasDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(FunctionDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(GeneratorDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(EnumDefinition const& statement, int parent_id)
{
}

void ASTVisualizer::visit(ExpressionStatement const& statement, int parent_id)
{
	visit(statement.expression, parent_id);
}

void ASTVisualizer::visit(AssertStatement const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"assert");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(ImploreStatement const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"implore");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(SwearStatement const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"swear");
	visit(statement.expression, id);
}

// Expression

void ASTVisualizer::visit(const Expression_ptr expression, int parent_id)
{
	std::visit(overloaded{
		[&](int const expr) { visit(expr, parent_id); },
		[&](double const expr) { visit(expr, parent_id); },
		[&](bool const expr) { visit(expr, parent_id); },
		[&](std::wstring const& expr) { visit(expr, parent_id); },
		[&](ListLiteral const& expr) { visit(expr, parent_id); },
		[&](TupleLiteral const& expr) { visit(expr, parent_id); },
		[&](SetLiteral const& expr) { visit(expr, parent_id); },
		[&](MapLiteral const& expr) { visit(expr, parent_id); },
		[&](UDTConstruct const& expr) { visit(expr, parent_id); },
		[&](UDTMemberAccess const& expr) { visit(expr, parent_id); },
		[&](EnumMember const& expr) { visit(expr, parent_id); },
		[&](Identifier const& expr) { visit(expr, parent_id); },
		[&](Call const& expr) { visit(expr, parent_id); },
		[&](Prefix const& expr) { visit(expr, parent_id); },
		[&](Infix const& expr) { visit(expr, parent_id); },
		[&](Postfix const& expr) { visit(expr, parent_id); },
		[&](Conditional const& expr) { visit(expr, parent_id); },
		[&](Assignment const& expr) { visit(expr, parent_id); },

		[](auto) {}
		}, *expression);
}

void ASTVisualizer::visit(int const expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, to_wstring(expr));
}

void ASTVisualizer::visit(double const expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, to_wstring(expr));
}

void ASTVisualizer::visit(bool const expr, int parent_id)
{
	const int id = id_counter++;

	if (expr)
	{
		save(id, parent_id, L"true");
	}
	else
	{
		save(id, parent_id, L"false");
	}
}

void ASTVisualizer::visit(std::wstring const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr);
}

void ASTVisualizer::visit(ListLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"List Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(TupleLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Tuple Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(SetLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Set Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(MapLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Map Literal");
}

void ASTVisualizer::visit(UDTConstruct const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.UDT_name);

	for (const auto element : expr.expressions)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(UDTMemberAccess const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"UDT member access");

	for (const auto element : expr.chain)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(EnumMember const& expr, int parent_id)
{
	std::wstring member_chain = L"";

	for (const auto member : expr.member_chain)
	{
		member_chain.append(member + L"::");
	}

	member_chain = member_chain.substr(0, member_chain.size() - 2);

	const int id = id_counter++;
	save(id, parent_id, member_chain);
}

void ASTVisualizer::visit(Identifier const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.name);
}

void ASTVisualizer::visit(Call const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.name);

	for (const auto element : expr.arguments)
	{
		visit(element, id);
	}
}

void ASTVisualizer::visit(Prefix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);
	visit(expr.operand, id);
}

void ASTVisualizer::visit(Infix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);

	visit(expr.left, id);
	visit(expr.right, id);
}

void ASTVisualizer::visit(Postfix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);

	visit(expr.operand, id);
}

void ASTVisualizer::visit(Conditional const& expr, int parent_id)
{
	const int question_id = id_counter++;
	save(question_id, parent_id, L"?");

	visit(expr.condition, question_id);
	const int colon_id = id_counter++;
	save(colon_id, question_id, L":");

	visit(expr.then_arm, colon_id);
	visit(expr.else_arm, colon_id);
}

void ASTVisualizer::visit(Assignment const& expr, int parent_id)
{
	const int equal_id = id_counter++;
	save(equal_id, parent_id, L"=");

	visit(expr.lhs_expression, equal_id);
	visit(expr.rhs_expression, equal_id);
}

// Generate

void ASTVisualizer::generate_dot_file(Module_ptr mod)
{
	// reset

	content = L"";
	id_counter = 0;

	// create content

	content.append(L"digraph G { \n");

	const int id = id_counter++;
	save(id, L"root");

	visit(mod->statements, id);

	content.append(L"}");

	// write to file

	std::string path = "../examples/expr_graph.dot";
	std::wofstream ofs(path, std::wofstream::out);
	ofs << content;
	ofs.close();
}