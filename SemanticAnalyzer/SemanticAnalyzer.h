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
#include "Builtins.h"
#include <memory>
#include <tuple>
#include <optional>

class SEMANTICANALYZER_API SemanticAnalyzer
{
	int next_id;

	TypeSystem_ptr type_system;
	SymbolScope_ptr current_scope;

	BuiltinsManager_ptr builtins_manager;

	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr>& statements);

	void visit(SingleVariableDefinition& statement);
	void visit(DeconstructedVariableDefinition& statement);
	void visit(ExpressionStatement& statement);
	void visit(SimpleIfBranch& statement);
	void visit(AssignedIfBranch& statement);
	void visit(ElseBranch& statement);
	void visit(SimpleWhileLoop& statement);
	void visit(AssignedWhileLoop& statement);
	void visit(Break& statement);
	void visit(Continue& statement);
	void visit(Redo& statement);
	void visit(Return& statement);
	void visit(Assert& statement);
	void visit(Implore& statement);
	void visit(Swear& statement);
	void visit(SimpleForInLoop& statement);
	void visit(DeconstructedForInLoop& statement);
	void visit(EnumDefinition& statement);
	void visit(FunctionDefinition& statement);
	void visit(AliasDefinition& statement);
	void visit(ClassDefinition& statement);
	void visit(Import& statement);
	void visit(Native& statement);

	// Expression

	Object_ptr visit(const Expression_ptr expr);
	ObjectVector visit(ExpressionVector expressions);

	Object_ptr visit(int expr);
	Object_ptr visit(double expr);
	Object_ptr visit(std::wstring expr);
	Object_ptr visit(bool expr);
	Object_ptr visit(ListLiteral& expr);
	Object_ptr visit(TupleLiteral& expr);
	Object_ptr visit(MapLiteral& expr);
	Object_ptr visit(SetLiteral& expr);
	Object_ptr visit(TernaryCondition& expr);
	Object_ptr visit(TypePattern& expr);
	Object_ptr visit(UntypedAssignment& expr);
	Object_ptr visit(TypedAssignment& expr);
	Object_ptr visit(Prefix& expr);
	Object_ptr visit(Infix& expr);
	Object_ptr visit(Postfix& expr);
	Object_ptr visit(Identifier& expr);
	Object_ptr visit(Call& expr);
	Object_ptr visit(EnumMember& expr);
	Object_ptr visit(Spread& expr);
	Object_ptr visit(TypeOf& expr);
	Object_ptr visit(Is& expr);
	Object_ptr visit(As& expr);

	Object_ptr infer_chain_member_type(Object_ptr lhs_operand_type, Expression_ptr expr, bool null_check_access);

	// Types

	Object_ptr visit(const TypeNode_ptr type_node);
	ObjectVector visit(std::vector<TypeNode_ptr>& type_nodes);

	Object_ptr visit(AnyTypeNode& expr);
	Object_ptr visit(IntLiteralTypeNode& expr);
	Object_ptr visit(FloatLiteralTypeNode& expr);
	Object_ptr visit(StringLiteralTypeNode& expr);
	Object_ptr visit(BooleanLiteralTypeNode& expr);
	Object_ptr visit(IntTypeNode& expr);
	Object_ptr visit(FloatTypeNode& expr);
	Object_ptr visit(StringTypeNode& expr);
	Object_ptr visit(BooleanTypeNode& expr);
	Object_ptr visit(ListTypeNode& expr);
	Object_ptr visit(TupleTypeNode& expr);
	Object_ptr visit(SetTypeNode& expr);
	Object_ptr visit(MapTypeNode& expr);
	Object_ptr visit(VariantTypeNode& expr);
	Object_ptr visit(NoneTypeNode& expr);
	Object_ptr visit(FunctionTypeNode& expr);
	Object_ptr visit(TypeIdentifierNode& expr);	

	// Utils

	void enter_scope(ScopeType scope_type);
	void leave_scope();

	std::tuple<std::wstring, Object_ptr> deconstruct_type_pattern(Expression_ptr expression);

	bool any_eq(ObjectVector vec, Object_ptr x);
	ObjectVector remove_duplicates(ObjectVector vec);

public:
	SemanticAnalyzer(BuiltinsManager_ptr builtins_manager)
		: next_id(10),
		type_system(std::make_shared<TypeSystem>()),
		builtins_manager(builtins_manager) {};

	void run(const Module_ptr ast);
};

using SemanticAnalyzer_ptr = SEMANTICANALYZER_API std::unique_ptr<SemanticAnalyzer>;
