#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Statement.h"
#include "TypeNode.h"
#include "Expression.h"
#include <string>
#include <memory>

class AST_API ASTVisualizer
{
	std::wstring content;
	int id_counter;

	void save(int id, std::wstring label);
	void save(int id, int parent, std::wstring label);

	std::wstring quote(std::wstring part);

	// Statement

	void visit(const Statement_ptr expr, int parent_id);
	void visit(std::vector<Statement_ptr> const& statements, int parent_id);

	void visit(IfBranch const& statement, int parent_id);
	void visit(ElseBranch const& statement, int parent_id);
	void visit(WhileLoop const& statement, int parent_id);
	void visit(ForInLoop const& statement, int parent_id);
	void visit(Break const& statement, int parent_id);
	void visit(Continue const& statement, int parent_id);
	void visit(Return const& statement, int parent_id);
	void visit(YieldStatement const& statement, int parent_id);
	void visit(VariableDefinition const& statement, int parent_id);
	void visit(ClassDefinition const& statement, int parent_id);
	void visit(InterfaceDefinition const& statement, int parent_id);
	void visit(AliasDefinition const& statement, int parent_id);
	void visit(FunctionDefinition const& statement, int parent_id);
	void visit(GeneratorDefinition const& statement, int parent_id);
	void visit(EnumDefinition const& statement, int parent_id);
	void visit(FunctionMemberDefinition const& statement, int parent_id);
	void visit(GeneratorMemberDefinition const& statement, int parent_id);
	void visit(ExpressionStatement const& statement, int parent_id);
	void visit(Assert const& statement, int parent_id);
	void visit(Implore const& statement, int parent_id);
	void visit(Swear const& statement, int parent_id);
	void visit(Namespace const& statement, int parent_id);
	void visit(InfixOperatorDefinition const& statement, int parent_id);
	void visit(PrefixOperatorDefinition const& statement, int parent_id);
	void visit(PostfixOperatorDefinition const& statement, int parent_id);

	// Expression

	void visit(const Expression_ptr expr, int parent_id);
	void visit(std::vector<Expression_ptr> const& expressions, int parent_id);

	void visit(int const expr, int parent_id);
	void visit(double const expr, int parent_id);
	void visit(bool const expr, int parent_id);
	void visit(std::wstring const& expr, int parent_id);
	void visit(ListLiteral const& expr, int parent_id);
	void visit(TupleLiteral const& expr, int parent_id);
	void visit(SetLiteral const& expr, int parent_id);
	void visit(MapLiteral const& expr, int parent_id);
	void visit(NewObject const& expr, int parent_id);
	void visit(EnumMember const& expr, int parent_id);
	void visit(Identifier const& expr, int parent_id);
	void visit(Call const& expr, int parent_id);
	void visit(Prefix const& expr, int parent_id);
	void visit(Infix const& expr, int parent_id);
	void visit(Postfix const& expr, int parent_id);
	void visit(TypePattern const& expr, int parent_id);
	void visit(Assignment const& expr, int parent_id);
	void visit(MemberAccess const& expr, int parent_id);
	void visit(TernaryCondition const& expr, int parent_id);

	// TypeNode

	void visit(const TypeNode_ptr type, int parent_id);
	void visit(std::vector<TypeNode_ptr> const& types, int parent_id);

	void visit(AnyTypeNode const& expr, int parent_id);
	void visit(IntLiteralTypeNode const& expr, int parent_id);
	void visit(FloatLiteralTypeNode const& expr, int parent_id);
	void visit(StringLiteralTypeNode const& expr, int parent_id);
	void visit(BooleanLiteralTypeNode const& expr, int parent_id);
	void visit(IntTypeNode const& expr, int parent_id);
	void visit(FloatTypeNode const& expr, int parent_id);
	void visit(StringTypeNode const& expr, int parent_id);
	void visit(BooleanTypeNode const& expr, int parent_id);
	void visit(ListTypeNode const& expr, int parent_id);
	void visit(TupleTypeNode const& expr, int parent_id);
	void visit(SetTypeNode const& expr, int parent_id);
	void visit(MapTypeNode const& expr, int parent_id);
	void visit(VariantTypeNode const& expr, int parent_id);
	void visit(NoneTypeNode const& expr, int parent_id);
	void visit(FunctionTypeNode const& expr, int parent_id);
	void visit(GeneratorTypeNode const& expr, int parent_id);
	void visit(FunctionMemberTypeNode const& expr, int parent_id);
	void visit(GeneratorMemberTypeNode const& expr, int parent_id);

public:
	ASTVisualizer()
		: content(L""), id_counter(0) {};

	void generate_dot_file(File_ptr mod);
};

using ASTVisualizer_ptr = AST_API std::unique_ptr<ASTVisualizer>;
