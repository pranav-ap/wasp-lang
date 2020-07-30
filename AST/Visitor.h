#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Expression.h"
#include "Statement.h"
#include <string>

class AST_API StatementVisitor
{
	virtual void visit(Statement_ptr statement) = 0;

	virtual void visit(Assignment statement) = 0;
	virtual void visit(Branching statement) = 0;
	virtual void visit(WhileLoop statement) = 0;
	virtual void visit(ForInLoop statement) = 0;
	virtual void visit(Break statement) = 0;
	virtual void visit(Continue statement) = 0;
	virtual void visit(Pass statement) = 0;
	virtual void visit(Return statement) = 0;
	virtual void visit(YieldStatement statement) = 0;
	virtual void visit(VariableDefinition statement) = 0;
	virtual void visit(UDTDefinition statement) = 0;
	virtual void visit(AliasDefinition statement) = 0;
	virtual void visit(FunctionDefinition statement) = 0;
	virtual void visit(GeneratorDefinition statement) = 0;
	virtual void visit(EnumDefinition statement) = 0;
	virtual void visit(ImportCustom statement) = 0;
	virtual void visit(ImportInBuilt statement) = 0;
	virtual void visit(ExpressionStatement statement) = 0;
	virtual void visit(AssertStatement statement) = 0;
};

class AST_API ExpressionVisitor
{
	virtual void visit(Expression_ptr expr) = 0;

	virtual void visit(double expr) = 0;
	virtual void visit(std::wstring expr) = 0;
	virtual void visit(bool expr) = 0;
	virtual void visit(ListLiteral expr) = 0;
	virtual void visit(TupleLiteral expr) = 0;
	virtual void visit(MapLiteral expr) = 0;
	virtual void visit(UDTConstruct expr) = 0;
	virtual void visit(UDTMemberAccess expr) = 0;
	virtual void visit(EnumMember expr) = 0;
	virtual void visit(Identifier expr) = 0;
	virtual void visit(Call expr) = 0;
	virtual void visit(Unary expr) = 0;
	virtual void visit(Binary expr) = 0;
};
