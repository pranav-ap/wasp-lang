#pragma once

#ifdef SEMANTICANALYZER_EXPORTS
#define SEMANTICANALYZER_API __declspec(dllexport)
#else
#define SEMANTICANALYZER_API __declspec(dllimport)
#endif

#include "SymbolTable.h"
#include "Statement.h"
#include "Visitor.h"
#include <memory>
#include <optional>

class SEMANTICANALYZER_API SemanticAnalyzer : public IStatementVisitor, public IExpressionVisitor
{
	std::optional<SymbolTable_ptr> current_symbol_table;

	// Statement

	void visit(Statement_ptr statement);
	void visit(std::vector<Statement_ptr> statements);

	void visit(Assignment& statement);
	void visit(Branching& statement);
	void visit(WhileLoop& statement);
	void visit(ForInLoop& statement);
	void visit(Break& statement);
	void visit(Continue& statement);
	void visit(Pass& statement);
	void visit(Return& statement);
	void visit(YieldStatement& statement);
	void visit(VariableDefinition& statement);
	void visit(UDTDefinition& statement);
	void visit(AliasDefinition& statement);
	void visit(FunctionDefinition& statement);
	void visit(GeneratorDefinition& statement);
	void visit(EnumDefinition& statement);
	void visit(ImportCustom& statement);
	void visit(ImportInBuilt& statement);
	void visit(ExpressionStatement& statement);
	void visit(AssertStatement& statement);

	// Expression

	void visit(Expression_ptr expr);
	void visit(std::vector<Expression_ptr> expressions);

	void visit(double expr);
	void visit(std::wstring expr);
	void visit(bool expr);
	void visit(ListLiteral& expr);
	void visit(TupleLiteral& expr);
	void visit(MapLiteral& expr);
	void visit(UDTConstruct& expr);
	void visit(UDTMemberAccess& expr);
	void visit(EnumMember& expr);
	void visit(Identifier& expr);
	void visit(Call& expr);
	void visit(Unary& expr);
	void visit(Binary& expr);

	void enter_scope(std::optional<SymbolTable_ptr> symbol_table);
	void leave_scope();

public:
	SemanticAnalyzer() {};
	void execute(Module_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::shared_ptr<SemanticAnalyzer>;
