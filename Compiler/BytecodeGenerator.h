#pragma once
#include "OpCode.h"
#include "Bytecode.h"
#include "Statement.h"
#include "Scope.h"
#include "ObjectSystem.h"
#include "CSymbolTable.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

class BytecodeGenerator
{
	std::stack<Scope_ptr> scope_stack;

	std::map<int, Object_ptr> constant_pool;
	int next_id;

	std::map<int, int> targets;
	int next_label;

	// Statement

	void visit(const Statement_ptr statement);
	void visit(std::vector<Statement_ptr> const& statements);

	void visit(Assignment const& statement);
	void visit(Branching const& statement);
	void visit(WhileLoop const& statement);
	void visit(ForInLoop const& statement);
	void visit(Pass const& statement);
	void visit(Break const& statement);
	void visit(Continue const& statement);
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

	Scope_ptr enter_scope();
	Instructions leave_scope();

	// Constant Pool

	int add_to_constant_pool(Object_ptr constant);

	int find_string_constant(std::wstring text);
	int find_number_constant(int number);

	// Emit

	void emit(Instruction instruction);

	void emit(OpCode opcode);
	void emit(OpCode opcode, int operand);
	void emit(OpCode opcode, int operand_1, int operand_2);

	// Make Instruction

	Instruction make_instruction(OpCode opcode);
	Instruction make_instruction(OpCode opcode, int operand);
	Instruction make_instruction(OpCode opcode, int operand_1, int operand_2);

	// Label

	int create_label();

public:
	BytecodeGenerator() : next_id(0), next_label(0) {};
	Bytecode_ptr execute(const Module_ptr module_ast);
};

using BytecodeGenerator_ptr = std::shared_ptr<BytecodeGenerator>;
