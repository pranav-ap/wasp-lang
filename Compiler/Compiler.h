#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "Statement.h"
#include "ObjectStore.h"
#include "Objects.h"
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <memory>

class COMPILER_API Compiler
{
	ObjectStore_ptr constant_pool; // Dont use for defintiions. Use id given by SA for definitions
	SymbolScope_ptr current_scope;
	
	std::map<int, std::wstring> name_map;
	int next_label;
		
	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr> const& statements);

	void visit(SingleVariableDefinition const& statement);
	void visit(DeconstructedVariableDefinition const& statement);
	void visit(EnumDefinition const& statement);

	void visit(ExpressionStatement const& statement);

	void visit(Assert const& statement);
	void visit(Implore const& statement);
	void visit(Swear const& statement);
	void visit(Return const& statement);
	void visit(YieldStatement const& statement);

	void visit(Scenario const& statement);
	void visit(Test const& statement);
	void visit(Namespace const& statement);

	// Branching

	void visit(SimpleIfBranch const& statement);
	void visit(SimpleIfBranch const& statement, int exit_tree_label, int branch_label);
	void visit(TaggedIfBranch const& statement);
	void visit(TaggedIfBranch const& statement, int exit_tree_label, int branch_label);
	void visit(ElseBranch const& statement);
	void visit(ElseBranch const& statement, int branch_label);

	// Looping 

	void visit(SimpleWhileLoop const& statement);
	void visit(AssignedWhileLoop const& statement);

	void visit(SimpleForInLoop const& statement);
	void visit(DeconstructedForInLoop const& statement);
	
	void visit(Break const& statement);
	void visit(Continue const& statement);
	void visit(Redo const& statement);
	
	// Expression

	void visit(const Expression_ptr expr);
	void visit(std::vector<Expression_ptr> const& expressions);

	void visit(const int expr);
	void visit(const double expr);
	void visit(const std::wstring expr);
	void visit(const bool expr);
	void visit(ListLiteral const& expr);
	void visit(TupleLiteral const& expr);
	void visit(MapLiteral const& expr);
	void visit(SetLiteral const& expr);
	void visit(TernaryCondition const& expr);
	void visit(TypePattern const& expr);
	void visit(UntypedAssignment const& expr);
	void visit(TypedAssignment const& expr);
	void visit(Prefix const& expr);
	void visit(Infix const& expr);
	void visit(Postfix const& expr);
	void visit(Identifier const& expr);
	void visit(Spread const& expr);
	void visit(EnumMember const& expr);

	// Emit

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	// Scope

	void set_current_scope(SymbolScope_ptr new_scope);
	ByteVector leave_scope();
	ByteVector leave_subroutine_scope();

	// Utils

	int create_label();
	std::wstring concat(StringVector items, std::wstring middle);
	std::wstring extract_identifier_from_type_pattern(Expression_ptr expression);

public:
	Compiler()
		: next_label(0), 
		current_scope(std::make_shared<SymbolScope>()),
		constant_pool(std::make_shared<ObjectStore>()) {};

	std::tuple<ObjectStore_ptr, CodeObject_ptr> run(const Module_ptr ast);
};

using Compiler_ptr = std::shared_ptr<Compiler>;

