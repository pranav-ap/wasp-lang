#pragma once

#ifdef COMPILER_EXPORTS
#define COMPILER_API __declspec(dllexport)
#else
#define COMPILER_API __declspec(dllimport)
#endif

#include "OpCode.h"
#include "Bytecode.h"
#include "Statement.h"
#include "Scope.h"
#include "ObjectSystem.h"
#include "CSymbolTable.h"
#include "MemorySystem.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>

class COMPILER_API Compiler
{
	MemorySystem_ptr memory;
	std::stack<Scope_ptr> scope_stack;

	int next_label;
	int next_memory_id;

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
	std::vector<std::byte> leave_scope();

	// Utils

	int define_variable(std::wstring name);
	int add_to_constant_pool(Object_ptr value);

	int create_label();

public:
	Compiler(MemorySystem_ptr memory)
		: next_label(0), next_memory_id(0),
		memory(memory) {};

	void execute(const Module_ptr module_ast);
};

using Compiler_ptr = std::shared_ptr<Compiler>;
