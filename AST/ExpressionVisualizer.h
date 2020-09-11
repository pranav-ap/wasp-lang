#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Expression.h"
#include <string>

class AST_API ExpressionVisualizer
{
	std::wstring content;
	int node_counter;

	std::wstring save(std::wstring part);
	std::wstring save(std::wstring stem, std::wstring part);

	std::wstring quote(std::wstring part);

	void visit(const Expression_ptr expr, std::wstring stem);

	void visit(int const expr, std::wstring stem);
	void visit(double const expr, std::wstring stem);
	void visit(bool const expr, std::wstring stem);
	void visit(std::wstring const& expr, std::wstring stem);
	void visit(ListLiteral const& expr, std::wstring stem);
	void visit(TupleLiteral const& expr, std::wstring stem);
	void visit(SetLiteral const& expr, std::wstring stem);
	void visit(MapLiteral const& expr, std::wstring stem);
	void visit(UDTConstruct const& expr, std::wstring stem);
	void visit(UDTMemberAccess const& expr, std::wstring stem);
	void visit(EnumMember const& expr, std::wstring stem);
	void visit(Identifier const& expr, std::wstring stem);
	void visit(Call const& expr, std::wstring stem);
	void visit(Prefix const& expr, std::wstring stem);
	void visit(Infix const& expr, std::wstring stem);
	void visit(Postfix const& expr, std::wstring stem);
	void visit(Conditional const& expr, std::wstring stem);

public:
	void gen_dot_file(Expression_ptr expression);
};

using ExpressionVisualizer_ptr = AST_API std::shared_ptr<ExpressionVisualizer>;
