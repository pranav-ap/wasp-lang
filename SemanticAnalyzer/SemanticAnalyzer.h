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

	void visit(Statement_ptr statement);
	void visit(std::vector<Statement_ptr>& statements);

	void visit(IfBranch& statement);
	void visit(ElseBranch& statement);
	void visit(WhileLoop& statement);
	void visit(ForInLoop& statement);
	void visit(Break& statement);
	void visit(Continue& statement);
	void visit(Redo& statement);
	void visit(Return& statement);
	void visit(YieldStatement& statement);
	void visit(VariableDefinition& statement);
	void visit(ClassDefinition& statement);
	void visit(InterfaceDefinition& statement);
	void visit(AliasDefinition& statement);
	void visit(FunctionDefinition& statement);
	void visit(GeneratorDefinition& statement);
	void visit(FunctionMemberDefinition& statement);
	void visit(GeneratorMemberDefinition& statement);
	void visit(EnumDefinition& statement);
	void visit(ExpressionStatement& statement);
	void visit(Assert& statement);
	void visit(Implore& statement);
	void visit(Swear& statement);
	void visit(Namespace& statement);

	// Expression

	Object_ptr visit(Expression_ptr expr);
	ObjectVector visit(std::vector<Expression_ptr>& expressions);

	Object_ptr visit(int expr);
	Object_ptr visit(double expr);
	Object_ptr visit(std::wstring expr);
	Object_ptr visit(bool expr);
	Object_ptr visit(ListLiteral& expr);
	Object_ptr visit(TupleLiteral& expr);
	Object_ptr visit(MapLiteral& expr);
	Object_ptr visit(SetLiteral& expr);
	Object_ptr visit(NewObject& expr);
	Object_ptr visit(Spread& expr);
	Object_ptr visit(TagPattern& expr);
	Object_ptr visit(Assignment& expr);
	Object_ptr visit(EnumMember& expr);
	Object_ptr visit(Call& expr);
	Object_ptr visit(Prefix& expr);
	Object_ptr visit(Infix& expr);
	Object_ptr visit(Postfix& expr);
	Object_ptr visit(Identifier& expr);
	Object_ptr visit(MemberAccess& expr);
	Object_ptr visit(TernaryCondition& expr);

	// Types

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
	std::tuple<std::wstring, Object_ptr> deconstruct_tag_pattern(Expression_ptr expression);

public:
	SemanticAnalyzer() :
		next_id(0),
		type_system(std::make_shared<TypeSystem>()) {};

	void execute(Module_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;


