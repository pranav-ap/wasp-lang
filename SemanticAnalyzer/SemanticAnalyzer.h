#pragma once

#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif

#include "Objects.h"
#include "TypeSystem.h"
#include "Statement.h"
#include "Expression.h"
#include "TypeNode.h"
#include "SymbolScope.h"
#include <memory>
#include <stack>
#include <tuple>
#include <optional>

class SEMANTICANALYZER_API SemanticAnalyzer
{
	int next_id;
	TypeSystem_ptr type_system;
	SymbolScope_ptr current_scope;

	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr> const& statements);

	void visit(IfBranch const& statement);
	void visit(ElseBranch const& statement);
	void visit(WhileLoop const& statement);
	void visit(ForInLoop const& statement);
	void visit(Break const& statement);
	void visit(Continue const& statement);
	void visit(Redo const& statement);
	void visit(Return const& statement);
	void visit(YieldStatement const& statement);
	void visit(VariableDefinition const& statement);
	void visit(ClassDefinition const& statement);
	void visit(InterfaceDefinition const& statement);
	void visit(AliasDefinition const& statement);
	void visit(FunctionDefinition const& statement);
	void visit(GeneratorDefinition const& statement);
	void visit(FunctionMemberDefinition const& statement);
	void visit(GeneratorMemberDefinition const& statement);
	void visit(EnumDefinition const& statement);
	void visit(ExpressionStatement const& statement);
	void visit(Assert const& statement);
	void visit(Implore const& statement);
	void visit(Swear const& statement);
	void visit(Namespace const& statement);

	// Expression

	Object_ptr visit(const ScopedExpression_ptr expr);
	Object_ptr visit(const Expression_ptr expr);
	ObjectVector visit(std::vector<Expression_ptr> const& expressions);

	Object_ptr visit(const int expr);
	Object_ptr visit(const double expr);
	Object_ptr visit(const std::wstring expr);
	Object_ptr visit(const bool expr);
	Object_ptr visit(ListLiteral const& expr);
	Object_ptr visit(TupleLiteral const& expr);
	Object_ptr visit(MapLiteral const& expr);
	Object_ptr visit(SetLiteral const& expr);
	Object_ptr visit(NewObject const& expr);
	Object_ptr visit(TernaryCondition const& expr);
	Object_ptr visit(Spread const& expr);
	Object_ptr visit(TypePattern const& expr);
	Object_ptr visit(Assignment const& expr);
	Object_ptr visit(EnumMember const& expr);
	Object_ptr visit(Call const& expr);
	Object_ptr visit(Prefix const& expr);
	Object_ptr visit(Infix const& expr);
	Object_ptr visit(Postfix const& expr);
	Object_ptr visit(Identifier const& expr);
	Object_ptr visit(MemberAccess const& expr);

	// Types

	Object_ptr visit(const TypeNode_ptr type_node);
	ObjectVector visit(std::vector<TypeNode_ptr> const& type_nodes);

	Object_ptr visit(AnyTypeNode const& expr);
	Object_ptr visit(IntLiteralTypeNode const& expr);
	Object_ptr visit(FloatLiteralTypeNode const& expr);
	Object_ptr visit(StringLiteralTypeNode const& expr);
	Object_ptr visit(BooleanLiteralTypeNode const& expr);
	Object_ptr visit(IntTypeNode const& expr);
	Object_ptr visit(FloatTypeNode const& expr);
	Object_ptr visit(StringTypeNode const& expr);
	Object_ptr visit(BooleanTypeNode const& expr);
	Object_ptr visit(ListTypeNode const& expr);
	Object_ptr visit(TupleTypeNode const& expr);
	Object_ptr visit(SetTypeNode const& expr);
	Object_ptr visit(MapTypeNode const& expr);
	Object_ptr visit(VariantTypeNode const& expr);
	Object_ptr visit(NoneTypeNode const& expr);
	Object_ptr visit(FunctionTypeNode const& expr);
	Object_ptr visit(GeneratorTypeNode const& expr);
	Object_ptr visit(FunctionMemberTypeNode const& expr);
	Object_ptr visit(GeneratorMemberTypeNode const& expr);
	Object_ptr visit(TypeIdentifierTypeNode const& expr);

	// Utils

	void enter_scope(ScopeType scope_type);	
	void leave_scope();

	std::wstring concat(StringVector items, std::wstring middle);
	std::tuple<std::wstring, Object_ptr> deconstruct_type_pattern(Expression_ptr expression);

public:
	SemanticAnalyzer();
	void execute(const File_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;


