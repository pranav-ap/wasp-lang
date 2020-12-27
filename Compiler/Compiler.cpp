#pragma once
#include "pch.h"
#include "Compiler.h"
#include "InstructionPrinter.h"
#include "Assertion.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <utility>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
#define MAKE_EXPRESSION(x) std::make_shared<Expression>(x)

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

using std::move;
using std::byte;
using std::wstring;
using std::map;
using std::make_shared;
using std::make_optional;
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;
using std::begin;
using std::end;

ByteVector Compiler::execute(const File_ptr ast)
{
	// Compile 

	set_current_scope(ast->scope);

	emit(OpCode::START);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	emit(OpCode::STOP);

	// Create CFG

	CFGBuilder_ptr cfg_builder = std::make_unique<CFGBuilder>(current_scope->code_object, object_store);
	CFG_ptr cfg = cfg_builder->create();

	// Assemble CFG

	ByteVector instructions = cfg_builder->assemble();

	InstructionPrinter_ptr printer = std::make_unique<InstructionPrinter>(object_store);
	printer->print(current_scope->code_object);

	return instructions;
}

// Statement

void Compiler::visit(const Statement_ptr statement)
{
	std::visit(overloaded{
		[&](IfBranch const& stat) { visit(stat); },
		[&](ElseBranch const& stat) { visit(stat); },
		[&](WhileLoop const& stat) { visit(stat); },
		[&](ForInLoop const& stat) { visit(stat); },
		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Redo const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](YieldStatement const& stat) { visit(stat); },
		[&](VariableDefinition const& stat) { visit(stat); },
		[&](ClassDefinition const& stat) { visit(stat); },
		[&](InterfaceDefinition const& stat) { visit(stat); },
		[&](AliasDefinition const& stat) { visit(stat); },
		[&](FunctionDefinition const& stat) { visit(stat); },
		[&](GeneratorDefinition const& stat) { visit(stat); },
		[&](FunctionMemberDefinition const& stat) { visit(stat); },
		[&](GeneratorMemberDefinition const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },
		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](Assert const& stat) { visit(stat); },
		[&](Implore const& stat) { visit(stat); },
		[&](Swear const& stat) { visit(stat); },
		[&](Namespace const& stat) { visit(stat); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& block)
{
}

void Compiler::visit(IfBranch const& statement)
{
	int exit_tree_label = create_label();
	int	branch_label = create_label();

	visit(statement, exit_tree_label, branch_label);
	emit(OpCode::LABEL, exit_tree_label);
}

void Compiler::visit(IfBranch const& statement, int exit_tree_label, int branch_label)
{
	emit(OpCode::LABEL, branch_label);

	set_current_scope(statement.scope);

	std::visit(overloaded{
		[&](Assignment const& expr)
		{
			wstring name;

			if (holds_alternative<Identifier>(*expr.lhs_expression))
			{
				auto identifier = get_if<Identifier>(&*expr.lhs_expression);
				name = identifier->name;
			}
			else
			{
				name = extract_identifier_from_type_pattern(expr.lhs_expression);
			}

			visit(expr.rhs_expression);

			int id = current_scope->lookup(name)->id;
			emit(OpCode::STORE_LOCAL, id);
			emit(OpCode::LOAD_LOCAL, id);
		},

		[&](auto)
		{
			visit(statement.test);
		}
		}, *statement.test);

	auto alternative = statement.alternative;

	if (alternative.has_value())
	{
		branch_label = create_label();
		emit(OpCode::POP_JUMP_IF_FALSE, branch_label);
	}
	else
	{
		emit(OpCode::POP_JUMP_IF_FALSE, exit_tree_label);
	}

	visit(statement.body);
	emit(OpCode::POP_JUMP, exit_tree_label);
	leave_scope();

	if (alternative.has_value())
	{
		if (holds_alternative<IfBranch>(*alternative.value()))
		{
			auto if_branch = get_if<IfBranch>(&*alternative.value());
			visit(*if_branch, exit_tree_label, branch_label);
		}
		else if (holds_alternative<ElseBranch>(*alternative.value()))
		{
			auto else_branch = get_if<ElseBranch>(&*alternative.value());
			visit(*else_branch, branch_label);
		}
		else
		{
			FATAL("Alternative must be an IfBranch or ElseBranch");
		}
	}
}

void Compiler::visit(ElseBranch const& statement)
{
	FATAL("Else must be part of an if branch");
}

void Compiler::visit(ElseBranch const& statement, int branch_label)
{
	set_current_scope(statement.scope);
	emit(OpCode::LABEL, branch_label);
	visit(statement.body);
	leave_scope();
}

void Compiler::visit(WhileLoop const& statement)
{
	set_current_scope(statement.scope);

	int condition_label = create_label();
	emit(OpCode::LABEL, condition_label);

	visit(statement.expression);

	current_scope->continue_label = condition_label;

	int block_end_label = create_label();
	emit(OpCode::POP_JUMP_IF_FALSE, block_end_label);
	current_scope->break_label = block_end_label;

	visit(statement.body);

	emit(OpCode::JUMP, condition_label);
	emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(ForInLoop const& statement)
{
	int iterate_label = create_label();
	emit(OpCode::LABEL, iterate_label);

	// Place iterable on stack

	auto iterable = statement.rhs_expression;
	visit(iterable);

	set_current_scope(statement.scope);

	int block_begin_label = create_label();
	emit(OpCode::LABEL, block_begin_label);
	current_scope->continue_label = block_begin_label;

	int block_end_label = create_label();
	current_scope->break_label = block_end_label;

	emit(OpCode::ITERATE_OVER, block_end_label);

	wstring name;

	if (holds_alternative<Identifier>(*statement.lhs_expression))
	{
		auto identifier = get_if<Identifier>(&*statement.lhs_expression);
		name = identifier->name;
	}
	else
	{
		name = extract_identifier_from_type_pattern(statement.lhs_expression);
	}

	int item_id = current_scope->lookup(name)->id;
	emit(OpCode::STORE_LOCAL, item_id);

	visit(statement.body);

	emit(OpCode::JUMP, block_begin_label);
	emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(Break const& statement)
{
	emit(OpCode::JUMP, current_scope->break_label);
}

void Compiler::visit(Continue const& statement)
{
	emit(OpCode::JUMP, current_scope->continue_label);
}

void Compiler::visit(Redo const& statement)
{
	// todo: reset loop
	emit(OpCode::JUMP, current_scope->continue_label);
}

void Compiler::visit(Return const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		emit(OpCode::RETURN_VALUE);
	}
	else
	{
		emit(OpCode::RETURN_VOID);
	}
}

void Compiler::visit(YieldStatement const& statement)
{
	if (statement.expression.has_value())
	{
		visit(statement.expression.value());
		emit(OpCode::YIELD_VALUE);
	}
	else
	{
		emit(OpCode::YIELD_VOID);
	}
}

void Compiler::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::POP_FROM_STACK);
}

void Compiler::visit(Assert const& statement)
{
	visit(statement.expression);
	emit(OpCode::ASSERT);
}

void Compiler::visit(Implore const& statement)
{
	visit(statement.expression);
	emit(OpCode::IMPLORE);
}

void Compiler::visit(Swear const& statement)
{
	visit(statement.expression);
	emit(OpCode::SWEAR);
}

void Compiler::visit(Namespace const& statement)
{
}
