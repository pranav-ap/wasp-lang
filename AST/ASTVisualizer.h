#pragma once

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#include "Statement.h"
#include "Type.h"
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

	void visit(Branching const& statement, int parent_id);
	void visit(WhileLoop const& statement, int parent_id);
	void visit(ForInLoop const& statement, int parent_id);
	void visit(Break const& statement, int parent_id);
	void visit(Continue const& statement, int parent_id);
	void visit(Return const& statement, int parent_id);
	void visit(YieldStatement const& statement, int parent_id);
	void visit(VariableDefinition const& statement, int parent_id);
	void visit(UDTDefinition const& statement, int parent_id);
	void visit(AliasDefinition const& statement, int parent_id);
	void visit(FunctionDefinition const& statement, int parent_id);
	void visit(GeneratorDefinition const& statement, int parent_id);
	void visit(EnumDefinition const& statement, int parent_id);
	void visit(ExpressionStatement const& statement, int parent_id);
	void visit(AssertStatement const& statement, int parent_id);
	void visit(ImploreStatement const& statement, int parent_id);
	void visit(SwearStatement const& statement, int parent_id);

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
	void visit(UDTConstruct const& expr, int parent_id);
	void visit(UDTMemberAccess const& expr, int parent_id);
	void visit(EnumMember const& expr, int parent_id);
	void visit(Identifier const& expr, int parent_id);
	void visit(Call const& expr, int parent_id);
	void visit(Prefix const& expr, int parent_id);
	void visit(Infix const& expr, int parent_id);
	void visit(Postfix const& expr, int parent_id);
	void visit(TypePattern const& expr, int parent_id);
	void visit(Assignment const& expr, int parent_id);

	// Types

	void visit(const Type_ptr type, int parent_id);
	void visit(std::vector<Type_ptr> const& types, int parent_id);

	void visit(AnyType const& expr, int parent_id);
	void visit(IntType const& expr, int parent_id);
	void visit(FloatType const& expr, int parent_id);
	void visit(StringType const& expr, int parent_id);
	void visit(BooleanType const& expr, int parent_id);
	void visit(ListType const& expr, int parent_id);
	void visit(TupleType const& expr, int parent_id);
	void visit(SetType const& expr, int parent_id);
	void visit(UDTType const& expr, int parent_id);
	void visit(MapType const& expr, int parent_id);
	void visit(EnumType const& expr, int parent_id);
	void visit(VariantType const& expr, int parent_id);
	void visit(NoneType const& expr, int parent_id);
	void visit(FunctionType const& expr, int parent_id);
	void visit(GeneratorType const& expr, int parent_id);

public:
	ASTVisualizer()
		: content(L""), id_counter(0) {};

	void generate_dot_file(Module_ptr mod);
};

using ASTVisualizer_ptr = AST_API std::unique_ptr<ASTVisualizer>;
