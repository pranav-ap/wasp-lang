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

void Compiler::visit(const Statement_ptr statement)
{
	std::visit(overloaded{
		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](SingleVariableDefinition const& stat) { visit(stat); },
		[&](DeconstructedVariableDefinition const& stat) { visit(stat); },
		[&](SimpleIfBranch const& stat) { visit(stat); },
		[&](AssignedIfBranch const& stat) { visit(stat); },
		[&](ElseBranch const& stat) { visit(stat); },
		[&](SimpleWhileLoop const& stat) { visit(stat); },
		[&](AssignedWhileLoop const& stat) { visit(stat); },
		[&](Break const& stat) { visit(stat); },
		[&](Continue const& stat) { visit(stat); },
		[&](Redo const& stat) { visit(stat); },
		[&](Return const& stat) { visit(stat); },
		[&](Assert const& stat) { visit(stat); },
		[&](Implore const& stat) { visit(stat); },
		[&](Swear const& stat) { visit(stat); },
		[&](SimpleForInLoop const& stat) { visit(stat); },
		[&](DeconstructedForInLoop const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },
		[&](FunctionDefinition const& stat) { visit(stat); },
		[&](Import const& stat) { visit(stat); },
		[&](Native const& stat) { visit(stat); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& statements)
{
	for (const auto stat : statements)
	{
		visit(stat);
	}
}

// Branching

void Compiler::visit(SimpleIfBranch const& statement)
{
	int exit_tree_label = create_label();
	int	branch_label = create_label();

	visit(statement, exit_tree_label, branch_label);
	emit(OpCode::LABEL, exit_tree_label);
}

void Compiler::visit(SimpleIfBranch const& statement, int exit_tree_label, int branch_label)
{
	set_current_scope(statement.scope);

	emit(OpCode::LABEL, branch_label);
	visit(statement.test);

	auto alternative_exists = statement.alternative.has_value();

	if (alternative_exists)
	{
		branch_label = create_label();

		emit(OpCode::POP_JUMP_IF_FALSE, branch_label);
		emit(OpCode::POP_FROM_STACK);
		emit(OpCode::PUSH_LOCAL_SCOPE);
		visit(statement.body);
		emit(OpCode::POP_LOCAL_SCOPE);

		emit(OpCode::JUMP, exit_tree_label);
	}
	else
	{
		emit(OpCode::POP_JUMP_IF_FALSE, exit_tree_label);
		emit(OpCode::POP_FROM_STACK);
		emit(OpCode::PUSH_LOCAL_SCOPE);
		visit(statement.body);
		emit(OpCode::POP_LOCAL_SCOPE);
	}

	leave_scope();

	if (alternative_exists)
	{
		std::visit(overloaded{
			[&](SimpleIfBranch const& if_branch) { visit(if_branch, exit_tree_label, branch_label); },
			[&](AssignedIfBranch const& if_branch) { visit(if_branch, exit_tree_label, branch_label); },
			[&](ElseBranch const& else_branch) { visit(else_branch, branch_label); },
			[](auto) { FATAL("Alternative must be an SimpleIfBranch or AssignedIfBranch or ElseBranch"); }
			}, *statement.alternative.value());
	}
}

void Compiler::visit(AssignedIfBranch const& statement)
{
	int exit_tree_label = create_label();
	int	branch_label = create_label();

	visit(statement, exit_tree_label, branch_label);
	emit(OpCode::LABEL, exit_tree_label);
}

void Compiler::visit(AssignedIfBranch const& statement, int exit_tree_label, int branch_label)
{
	set_current_scope(statement.scope);
	emit(OpCode::LABEL, branch_label);

	visit(statement.rhs_expression);

	int symbol_id = current_scope->lookup(statement.name)->id;
	int id = create_pool_id(symbol_id);

	name_map[id] = statement.name;
	id_map[symbol_id] = id;

	auto alternative_exists = statement.alternative.has_value();

	if (alternative_exists)
	{
		branch_label = create_label();
		emit(OpCode::POP_JUMP_IF_FALSE, branch_label);
	}
	else
	{
		emit(OpCode::POP_JUMP_IF_FALSE, exit_tree_label);
	}

	emit(OpCode::PUSH_LOCAL_SCOPE);
	emit(OpCode::CREATE_LOCAL, id);

	visit(statement.body);

	emit(OpCode::POP_LOCAL_SCOPE);

	if (alternative_exists)
	{
		emit(OpCode::JUMP, exit_tree_label);
	}

	leave_scope();

	if (alternative_exists)
	{
		std::visit(overloaded{
			[&](SimpleIfBranch const& if_branch) { visit(if_branch, exit_tree_label, branch_label); },
			[&](AssignedIfBranch const& if_branch) { visit(if_branch, exit_tree_label, branch_label); },
			[&](ElseBranch const& else_branch) { visit(else_branch, branch_label); },
			[](auto) { FATAL("Alternative must be an SimpleIfBranch or AssignedIfBranch or ElseBranch"); }
			}, *statement.alternative.value());
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
	emit(OpCode::PUSH_LOCAL_SCOPE);

	visit(statement.body);

	emit(OpCode::POP_LOCAL_SCOPE);

	leave_scope();
}

// Looping

void Compiler::visit(SimpleWhileLoop const& statement)
{
	set_current_scope(statement.scope);
	emit(OpCode::PUSH_LOCAL_SCOPE);

	int condition_label = create_label();
	emit(OpCode::LABEL, condition_label);

	visit(statement.test);

	current_scope->continue_label = condition_label;

	int block_end_label = create_label();
	emit(OpCode::POP_JUMP_IF_FALSE, block_end_label);
	current_scope->break_label = block_end_label;

	visit(statement.body);

	emit(OpCode::JUMP, condition_label);
	emit(OpCode::LABEL, block_end_label);

	emit(OpCode::POP_LOCAL_SCOPE);
	leave_scope();
}

void Compiler::visit(AssignedWhileLoop const& statement)
{
}

void Compiler::visit(SimpleForInLoop const& statement)
{
	set_current_scope(statement.scope);
	emit(OpCode::PUSH_LOCAL_SCOPE);

	visit(statement.iterable_expression);
	emit(OpCode::MAKE_ITERABLE);

	int block_begin_label = create_label();
	emit(OpCode::LABEL, block_begin_label);
	current_scope->continue_label = block_begin_label;

	int block_end_label = create_label();
	current_scope->break_label = block_end_label;

	emit(OpCode::GET_NEXT_OR_JUMP, block_end_label);

	int symbol_id = current_scope->lookup(statement.name)->id;
	int id = create_pool_id(symbol_id);

	emit(OpCode::CREATE_LOCAL, id);

	name_map[id] = statement.name;
	id_map[symbol_id] = id;

	visit(statement.body);

	emit(OpCode::JUMP, block_begin_label);
	emit(OpCode::LABEL, block_end_label);

	emit(OpCode::POP_LOCAL_SCOPE);
	leave_scope();
}

void Compiler::visit(DeconstructedForInLoop const& statement)
{
}

// Block statements

void Compiler::visit(Native const& statement)
{
	// do nothing
}

// Other stuff

void Compiler::visit(ExpressionStatement const& statement)
{
	visit(statement.expression);
	emit(OpCode::POP_FROM_STACK);
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
	// reset iterator
	emit(OpCode::JUMP, current_scope->continue_label);
}