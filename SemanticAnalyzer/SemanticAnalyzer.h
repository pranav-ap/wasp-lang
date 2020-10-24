#pragma once

#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif

#include "Type.h"
#include "TypeSystem.h"
#include "SymbolScope.h"
#include "Statement.h"
#include "Expression.h"
#include <memory>
#include <stack>
#include <optional>

class SEMANTICANALYZER_API SemanticAnalyzer
{
	SymbolScope_ptr current_scope;
	TypeSystem_ptr type_system;

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
	void visit(FunctionMethodDefinition const& statement);
	void visit(GeneratorMethodDefinition const& statement);
	void visit(EnumDefinition const& statement);
	void visit(ExpressionStatement const& statement);
	void visit(Assert const& statement);
	void visit(Implore const& statement);
	void visit(Swear const& statement);
	void visit(Module const& statement);
	void visit(InfixOperatorDefinition const& statement);
	void visit(PrefixOperatorDefinition const& statement);
	void visit(PostfixOperatorDefinition const& statement);

	// Expression

	Type_ptr visit(const Expression_ptr expr);

	Type_ptr visit(const double expr);
	Type_ptr visit(const std::wstring expr);
	Type_ptr visit(const bool expr);
	Type_ptr visit(ListLiteral const& expr);
	Type_ptr visit(TupleLiteral const& expr);
	Type_ptr visit(MapLiteral const& expr);
	Type_ptr visit(SetLiteral const& expr);
	Type_ptr visit(NewObject const& expr);
	Type_ptr visit(TernaryCondition const& expr);
	Type_ptr visit(SpreadExpression const& expr);
	Type_ptr visit(TypePattern const& expr);
	Type_ptr visit(Assignment const& expr);
	Type_ptr visit(EnumMember const& expr);
	Type_ptr visit(Call const& expr);
	Type_ptr visit(Prefix const& expr);
	Type_ptr visit(Infix const& expr);
	Type_ptr visit(Postfix const& expr);
	Type_ptr visit(Identifier const& expr);

	// Utils

	void enter_scope(ScopeType scope_type);
	void leave_scope();

public:
	SemanticAnalyzer() {};
	void execute(const File_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;
