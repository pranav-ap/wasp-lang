#pragma once
#include "pch.h"
#include "Compiler.h"
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

	this->compile(ast);

	// Create CFG

	//CFGBuilder_ptr cfg_builder = make_unique<CFGBuilder>();
	//CFG_ptr cfg = cfg_builder->execute();

	// Assemble

	//ByteVector instructions = cfg_builder->assemble();
}

void Compiler::compile(const File_ptr ast)
{
	set_current_scope(ast->scope);

	emit(OpCode::START);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	emit(OpCode::STOP);
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
			ASSERT(holds_alternative<TypePattern>(*expr.lhs_expression), "Expected type pattern");
			auto type_pattern = get_if<TypePattern>(&*expr.lhs_expression);

			ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
			auto identifier = get_if<Identifier>(&*type_pattern->expression);

			visit(expr.rhs_expression);

			int id = current_scope->lookup(identifier->name)->id;
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

	ASSERT(holds_alternative<TypePattern>(*statement.lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*statement.lhs_expression);
	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	int item_id = current_scope->lookup(identifier->name)->id;
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

void Compiler::visit(VariableDefinition const& statement)
{
	visit(statement.rhs_expression);

	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	int id =  current_scope->lookup(identifier->name)->id;
	emit(OpCode::STORE_LOCAL, id);

	object_store->name_map[id] = identifier->name;
}

void Compiler::visit(ClassDefinition const& statement)
{
	map<wstring, int> members;

	for (auto const& [member_name, type] : statement.member_types)
	{
		int id = current_scope->lookup(statement.name + L"::" + member_name)->id;
		members.insert({ member_name , id });
	}

	auto class_symbol = current_scope->lookup(statement.name);
	auto class_id = class_symbol->id;
	auto class_object = MAKE_OBJECT_VARIANT(ClassObject(statement.name, members, class_symbol->type));
	object_store->set(class_id, move(class_object));
	object_store->name_map.insert({ class_id, statement.name });
}

void Compiler::visit(InterfaceDefinition const& statement)
{
	// Do nothing
}

void Compiler::visit(AliasDefinition const& statement)
{
	// Do nothing
}

void Compiler::visit(FunctionDefinition const& statement)
{
	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto function_symbol = current_scope->lookup(statement.name);
	auto function_id = function_symbol->id;
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(statement.name, instructions, parameter_count, function_symbol->type));
	object_store->set(function_id, move(function_object));
	object_store->name_map.insert({ function_id, statement.name });
}

void Compiler::visit(GeneratorDefinition const& statement)
{
	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto generator_symbol = current_scope->lookup(statement.name);
	auto generator_id = generator_symbol->id;
	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorObject(statement.name, instructions, parameter_count, generator_symbol->type));
	object_store->set(generator_id, move(generator_object));
	object_store->name_map.insert({ generator_id, statement.name });
}

void Compiler::visit(FunctionMemberDefinition const& statement)
{
	/*int id = current_scope->lookup(statement.type_name)->id;

	Object_ptr object = object_store->get(id);
	ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	auto class_object = get_if<ClassObject>(&*object);

	ASSERT(class_object->members.contains(statement.name), "Method name is not found in class definition");
	int method_id = class_object->members.at(statement.name);

	set_current_scope(statement.scope);

	for (auto const& arg_name : statement.arguments)
	{
		int id = current_scope->lookup(arg_name)->id;
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto type = MAKE_OBJECT_VARIANT(FunctionMemberType(statement.name, statement.members));

	auto function_object = MAKE_OBJECT_VARIANT(FunctionMethodObject(statement.name, instructions, parameter_count, type));
	object_store->set(method_id, move(function_object));
	object_store->name_map.insert({ method_id, statement.name });*/
}

void Compiler::visit(GeneratorMemberDefinition const& statement)
{
	//int id = current_scope->lookup(statement.type_name)->id;

	//Object_ptr object = object_store->get(id);
	//ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	//auto class_object = get_if<ClassObject>(&*object);

	//ASSERT(class_object->members.contains(statement.name), "Method name is not found in class definition");
	//int method_id = class_object->members.at(statement.name);

	//set_current_scope(statement.scope);

	//for (auto const& arg_name : statement.arguments)
	//{
	//	int id = current_scope->lookup(arg_name)->id;
	//	emit(OpCode::STORE_LOCAL, id);
	//}

	//visit(statement.body);

	//auto instructions = leave_subroutine_scope();
	//int parameter_count = statement.arguments.size();

	//Symbol_ptr class_symbol = current_scope->lookup(statement.name);
	//auto generator_object = MAKE_OBJECT_VARIANT(GeneratorMethodObject(statement.name, instructions, parameter_count, type));
	//object_store->set(method_id, move(generator_object));
	//object_store->name_map.insert({ method_id, statement.name });
}

void Compiler::visit(EnumDefinition const& statement)
{
	auto type = MAKE_OBJECT_VARIANT(EnumType(statement.name, statement.members));
	auto enum_object = MAKE_OBJECT_VARIANT(EnumObject(statement.name, statement.members, type));

	auto enum_id = current_scope->lookup(statement.name)->id;
	object_store->set(enum_id, move(enum_object));
	object_store->name_map.insert({ enum_id, statement.name });
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
