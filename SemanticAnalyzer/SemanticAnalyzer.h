#pragma once

#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif

#include "SymbolTable.h"
#include "Statement.h"
#include <memory>
#include <optional>

class SEMANTICANALYZER_API SemanticAnalyzer
{
	SymbolTable_ptr symbol_table;

	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr> const& statements);

	void visit(Assignment const& statement);
	void visit(Branching const& statement);
	void visit(WhileLoop const& statement);
	void visit(ForInLoop const& statement);
	void visit(Break const& statement);
	void visit(Continue const& statement);
	void visit(Pass const& statement);
	void visit(Return const& statement);
	void visit(YieldStatement const& statement);
	void visit(VariableDefinition const& statement);
	void visit(UDTDefinition const& statement);
	void visit(AliasDefinition const& statement);
	void visit(FunctionDefinition const& statement);
	void visit(GeneratorDefinition const& statement);
	void visit(EnumDefinition const& statement);
	void visit(ImportCustom const& statement);
	void visit(ImportInBuilt const& statement);
	void visit(ExpressionStatement const& statement);
	void visit(AssertStatement const& statement);

	// Expression

	void visit(const Expression_ptr expr);
	void visit(std::vector<Expression_ptr> const& expressions);

	void visit(const double expr);
	void visit(const std::wstring expr);
	void visit(const bool expr);
	void visit(ListLiteral const& expr);
	void visit(TupleLiteral const& expr);
	void visit(MapLiteral const& expr);
	void visit(UDTConstruct const& expr);
	void visit(UDTMemberAccess const& expr);
	void visit(EnumMember const& expr);
	void visit(Identifier const& expr);
	void visit(Call const& expr);
	void visit(Unary const& expr);
	void visit(Binary const& expr);

	// Utils

	void enter_scope();
	void leave_scope();

public:
	SemanticAnalyzer() {};
	void execute(const Module_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;
