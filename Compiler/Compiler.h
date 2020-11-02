#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "CScope.h"
#include "Statement.h"
#include "ObjectSystem.h"
#include "MemorySystem.h"

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <memory>
#include <optional>

class COMPILER_API Compiler
{
	MemorySystem_ptr memory;
	CScope_ptr current_scope;

	int next_label;
	int next_id;

	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr> const& statements);

	void visit(IfBranch const& statement);
	void visit(IfBranch const& stat, int exit_tree_label, int branch_label);

	void visit(ElseBranch const& statement);
	void visit(ElseBranch const& statement, int branch_label);

	void visit(WhileLoop const& statement);
	void visit(ForInLoop const& statement);

	void visit(Break const& statement);
	void visit(Continue const& statement);

	void visit(Return const& statement);
	void visit(YieldStatement const& statement);

	void visit(VariableDefinition const& statement);
	void visit(InterfaceDefinition const& statement);
	void visit(ClassDefinition const& statement);
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
	void visit(NewObject const& expr);
	void visit(TernaryCondition const& expr);
	void visit(TypePattern const& expr);
	void visit(Assignment const& expr);
	void visit(EnumMember const& expr);
	void visit(Call const& expr);
	void visit(Prefix const& expr);
	void visit(Infix const& expr);
	void visit(Postfix const& expr);
	void visit(Identifier const& expr);
	void visit(Spread const& expr);
	void visit(MemberAccess const& expr);

	// Emit

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	// Scope

	void enter_scope();
	ByteVector leave_scope();
	ByteVector leave_subroutine_scope();

	// Utils

	int define(std::wstring name);
	int create_label();

	std::wstring concat(StringVector items, std::wstring middle);
	std::wstring deconstruct_type_pattern(Expression_ptr expression);

public:
	Compiler(MemorySystem_ptr memory)
		: next_label(0), next_id(0), memory(memory) {};

	void execute(const File_ptr ast);
};

using Compiler_ptr = std::shared_ptr<Compiler>;
