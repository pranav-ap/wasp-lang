#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "Bytecode.h"
#include "Statement.h"
#include "ObjectSystem.h"
#include "SymbolTable.h"

#include <string>
#include <vector>
#include <memory>

struct CompilationScope
{
	Instructions instructions;
};

using CompilationScope_ptr = std::shared_ptr<CompilationScope>;

class COMPILER_API Compiler
{
	std::stack<CompilationScope_ptr> scopes;
	std::vector<Object_ptr> constant_pool;

	std::vector<int> labels;
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

	// Scope

	void enter_scope();
	Instructions leave_scope();

	// Emit

	int emit(OpCode opcode);
	int emit(OpCode opcode, int operand);
	int emit(OpCode opcode, int operand_1, int operand_2);

	// Utils

	int add_to_constant_pool(Object_ptr constant);

	Instruction make_instruction(OpCode opcode);
	Instruction make_instruction(OpCode opcode, int operand);
	Instruction make_instruction(OpCode opcode, int operand_1, int operand_2);

public:
	Compiler()
		: scopes({ std::make_shared<CompilationScope>() }),
		symbol_table(std::make_shared<SymbolTable>()) {};

	Bytecode_ptr execute(const Module_ptr module_ast);
};

using Compiler_ptr = COMPILER_API std::shared_ptr<Compiler>;
