#pragma once

#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif

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
	void visit(InfixOperatorDefinition const& statement);
	void visit(PrefixOperatorDefinition const& statement);
	void visit(PostfixOperatorDefinition const& statement);

	// Expression

	TypeNode_ptr visit(const Expression_ptr expr);
	TypeNodeVector visit(std::vector<Expression_ptr> const& expressions);

	TypeNode_ptr visit(const int expr);
	TypeNode_ptr visit(const double expr);
	TypeNode_ptr visit(const std::wstring expr);
	TypeNode_ptr visit(const bool expr);
	TypeNode_ptr visit(ListLiteral const& expr);
	TypeNode_ptr visit(TupleLiteral const& expr);
	TypeNode_ptr visit(MapLiteral const& expr);
	TypeNode_ptr visit(SetLiteral const& expr);
	TypeNode_ptr visit(NewObject const& expr);
	TypeNode_ptr visit(TernaryCondition const& expr);
	TypeNode_ptr visit(Spread const& expr);
	TypeNode_ptr visit(TypePattern const& expr);
	TypeNode_ptr visit(Assignment const& expr);
	TypeNode_ptr visit(EnumMember const& expr);
	TypeNode_ptr visit(Call const& expr);
	TypeNode_ptr visit(Prefix const& expr);
	TypeNode_ptr visit(Infix const& expr);
	TypeNode_ptr visit(Postfix const& expr);
	TypeNode_ptr visit(Identifier const& expr);
	TypeNode_ptr visit(MemberAccess const& expr);

	// Utils

	void enter_scope(ScopeType scope_type);
	void leave_scope();

	std::wstring concat(StringVector items, std::wstring middle);
	std::tuple<std::wstring, TypeNode_ptr> deconstruct_type_pattern(Expression_ptr expression);

public:
	SemanticAnalyzer();
	void execute(const File_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;
