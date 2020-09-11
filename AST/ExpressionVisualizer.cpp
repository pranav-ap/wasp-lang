#pragma once
#include "pch.h"
#include "ExpressionVisualizer.h"
#include <variant>
#include <string>
#include <fstream>

#define ARROW L" -> "

using std::to_wstring;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

std::wstring ExpressionVisualizer::save(std::wstring part)
{
	std::wstring stem = to_wstring(node_counter++);
	content.append(stem + L"\n");
	content.append(stem + L"[label=" + quote(part) + L"] \n");
	return stem;
}

std::wstring ExpressionVisualizer::save(std::wstring stem, std::wstring part)
{
	stem = stem + ARROW + to_wstring(node_counter);
	content.append(stem + L"\n");
	content.append(to_wstring(node_counter++) + L"[label=" + quote(part) + L"] \n");
	return stem;
}

std::wstring ExpressionVisualizer::quote(std::wstring part)
{
	return L"\"" + part + L"\"";
}

void ExpressionVisualizer::gen_dot_file(Expression_ptr expression)
{
	content.append(L"digraph G { \n");

	std::wstring stem = L"\"root\"";
	visit(expression, stem);

	content.append(L"}");

	std::string path = "../examples/expr_graph.dot";
	std::wofstream ofs(path, std::wofstream::out);
	ofs << content;
	ofs.close();
}

void ExpressionVisualizer::visit(const Expression_ptr expression, std::wstring stem)
{
	std::visit(overloaded{
		[&](int const expr) { visit(expr, stem); },
		[&](double const expr) { visit(expr, stem); },
		[&](bool const expr) { visit(expr, stem); },
		[&](std::wstring const& expr) { visit(expr, stem); },
		[&](ListLiteral const& expr) { visit(expr, stem); },
		[&](TupleLiteral const& expr) { visit(expr, stem); },
		[&](SetLiteral const& expr) { visit(expr, stem); },
		[&](MapLiteral const& expr) { visit(expr, stem); },
		[&](UDTConstruct const& expr) { visit(expr, stem); },
		[&](UDTMemberAccess const& expr) { visit(expr, stem); },
		[&](EnumMember const& expr) { visit(expr, stem); },
		[&](Identifier const& expr) { visit(expr, stem); },
		[&](Call const& expr) { visit(expr, stem); },
		[&](Prefix const& expr) { visit(expr, stem); },
		[&](Infix const& expr) { visit(expr, stem); },
		[&](Postfix const& expr) { visit(expr, stem); },
		[&](Conditional const& expr) { visit(expr, stem); },

		[](auto) {}
		}, *expression);
}

void ExpressionVisualizer::visit(int const expr, std::wstring stem)
{
	save(stem, to_wstring(expr));
}

void ExpressionVisualizer::visit(double const expr, std::wstring stem)
{
	save(stem, to_wstring(expr));
}

void ExpressionVisualizer::visit(bool const expr, std::wstring stem)
{
	save(stem, to_wstring(expr));
}

void ExpressionVisualizer::visit(std::wstring const& expr, std::wstring stem)
{
	save(stem, expr);
}

void ExpressionVisualizer::visit(ListLiteral const& expr, std::wstring stem)
{
	stem.append(L" -> List Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, stem);
	}
}

void ExpressionVisualizer::visit(TupleLiteral const& expr, std::wstring stem)
{
	stem.append(L" -> Tuple Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, stem);
	}
}

void ExpressionVisualizer::visit(SetLiteral const& expr, std::wstring stem)
{
	stem.append(L" -> Set Literal");

	for (const auto element : expr.expressions)
	{
		visit(element, stem);
	}
}

void ExpressionVisualizer::visit(MapLiteral const& expr, std::wstring stem)
{
	for (const auto [key, value] : expr.pairs)
	{
	}
}

void ExpressionVisualizer::visit(UDTConstruct const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(UDTMemberAccess const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(EnumMember const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(Identifier const& expr, std::wstring stem)
{
	save(stem, expr.name);
}

void ExpressionVisualizer::visit(Call const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(Prefix const& expr, std::wstring stem)
{
	stem = save(stem, expr.op->value);
	visit(expr.operand, stem);
}

void ExpressionVisualizer::visit(Infix const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(Postfix const& expr, std::wstring stem)
{
}

void ExpressionVisualizer::visit(Conditional const& expr, std::wstring stem)
{
}