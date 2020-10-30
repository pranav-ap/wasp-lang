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
using std::holds_alternative;
using std::get_if;
using std::vector;
using std::to_wstring;

void Compiler::execute(const File_ptr ast)
{
	next_label = 0;

	enter_scope();

	emit(OpCode::START);

	for (auto statement : ast->statements)
	{
		visit(statement);
	}

	emit(OpCode::STOP);

	leave_scope();
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
		[&](FunctionMethodDefinition const& stat) { visit(stat); },
		[&](GeneratorMethodDefinition const& stat) { visit(stat); },
		[&](EnumDefinition const& stat) { visit(stat); },
		[&](ExpressionStatement const& stat) { visit(stat); },
		[&](Assert const& stat) { visit(stat); },
		[&](Implore const& stat) { visit(stat); },
		[&](Swear const& stat) { visit(stat); },
		[&](Namespace const& stat) { visit(stat); },
		[&](InfixOperatorDefinition const& stat) { visit(stat); },
		[&](PrefixOperatorDefinition const& stat) { visit(stat); },
		[&](PostfixOperatorDefinition const& stat) { visit(stat); },

		[](auto) { FATAL("Never Seen this Statement before!"); }
		}, *statement);
}

void Compiler::visit(std::vector<Statement_ptr> const& block)
{
	for (const auto stat : block)
	{
		visit(stat);
	}
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

	enter_scope();

	std::visit(overloaded{
		[&](Assignment const& expr)
		{
			ASSERT(holds_alternative<TypePattern>(*expr.lhs_expression), "Expected type pattern");
			auto type_pattern = get_if<TypePattern>(&*expr.lhs_expression);

			ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
			auto identifier = get_if<Identifier>(&*type_pattern->expression);

			visit(expr.rhs_expression);

			int id = define(identifier->name);
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
	enter_scope();
	emit(OpCode::LABEL, branch_label);
	visit(statement.body);
	leave_scope();
}

void Compiler::visit(WhileLoop const& statement)
{
	int condition_label = create_label();
	emit(OpCode::LABEL, condition_label);

	visit(statement.expression);

	auto scope = enter_scope();
	scope->continue_label = condition_label;

	int block_end_label = create_label();
	emit(OpCode::POP_JUMP_IF_FALSE, block_end_label);
	scope->break_label = block_end_label;

	visit(statement.block);

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

	auto scope = enter_scope();

	int block_begin_label = create_label();
	emit(OpCode::LABEL, block_begin_label);
	scope->continue_label = block_begin_label;

	int block_end_label = create_label();
	scope->break_label = block_end_label;

	std::visit(overloaded{
		[&](wstring const& expr) { emit(OpCode::ITERATE_OVER_STRING, block_end_label); },
		[&](ListLiteral const& expr) { emit(OpCode::ITERATE_OVER_LIST, block_end_label); },
		[&](MapLiteral const& expr) { emit(OpCode::ITERATE_OVER_MAP, block_end_label); },
		[&](Identifier const& expr) { emit(OpCode::ITERATE_OVER_IDENTIFIER, block_end_label); },

		[](auto) { FATAL("Not an iterable!"); }
		}, *iterable);

	ASSERT(holds_alternative<TypePattern>(*statement.lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*statement.lhs_expression);
	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	int item_id = define(identifier->name);
	emit(OpCode::STORE_LOCAL, item_id);

	visit(statement.block);

	emit(OpCode::JUMP, block_begin_label);
	emit(OpCode::LABEL, block_end_label);

	leave_scope();
}

void Compiler::visit(Break const& statement)
{
	auto scope = scope_stack.top();
	emit(OpCode::JUMP, scope->break_label);
}

void Compiler::visit(Continue const& statement)
{
	auto scope = scope_stack.top();
	emit(OpCode::JUMP, scope->continue_label);
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
	ASSERT(holds_alternative<Assignment>(*statement.expression), "Must be an Assignment");
	auto assignment = get_if<Assignment>(&*statement.expression);

	visit(assignment->rhs_expression);

	ASSERT(holds_alternative<TypePattern>(*assignment->lhs_expression), "Must be a TypePattern");
	auto type_pattern = get_if<TypePattern>(&*assignment->lhs_expression);

	ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*type_pattern->expression);

	int id = define(identifier->name);
	emit(OpCode::STORE_LOCAL, id);
}

void Compiler::visit(ClassDefinition const& statement)
{
	map<wstring, int> static_fields;
	map<wstring, int> methods;

	for (auto const& [member_name, type] : statement.member_types)
	{
		int id = define(statement.name + L"::" + member_name);
		methods.insert({ member_name , id });
	}

	auto class_id = define(statement.name);
	auto class_object = MAKE_OBJECT_VARIANT(ClassObject(statement.name, static_fields, methods));
	memory->definition_store->set(class_id, move(class_object));
}

void Compiler::visit(InterfaceDefinition const& statement)
{
	// Do nothing
}

void Compiler::visit(AliasDefinition const& statement)
{
}

void Compiler::visit(FunctionDefinition const& statement)
{
	enter_scope();

	for (auto const& arg_name : statement.arguments)
	{
		int id = define(arg_name);
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.block);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto function_id = define(statement.name);
	auto function_object = MAKE_OBJECT_VARIANT(FunctionObject(statement.name, instructions, parameter_count));
	memory->definition_store->set(function_id, move(function_object));
}

void Compiler::visit(GeneratorDefinition const& statement)
{
	enter_scope();

	for (auto const& arg_name : statement.arguments)
	{
		int id = define(arg_name);
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.block);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto generator_id = define(statement.name);
	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorObject(statement.name, instructions, parameter_count));
	memory->definition_store->set(generator_id, move(generator_object));
}

void Compiler::visit(FunctionMethodDefinition const& statement)
{
	auto scope = scope_stack.top();
	int id = scope->symbol_table->lookup(statement.type_name);

	Object_ptr object = memory->definition_store->get(id);
	ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	auto class_object = get_if<ClassObject>(&*object);

	ASSERT(class_object->methods.contains(statement.name), "Method name is not found in class definition");
	int method_id = class_object->methods.at(statement.name);

	enter_scope();

	for (auto const& arg_name : statement.arguments)
	{
		int id = define(arg_name);
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto function_object = MAKE_OBJECT_VARIANT(FunctionMethodObject(statement.name, instructions, parameter_count));
	memory->definition_store->set(method_id, move(function_object));
}

void Compiler::visit(GeneratorMethodDefinition const& statement)
{
	auto scope = scope_stack.top();
	int id = scope->symbol_table->lookup(statement.type_name);

	Object_ptr object = memory->definition_store->get(id);
	ASSERT(holds_alternative<ClassObject>(*object), "Expected class defintion object");
	auto class_object = get_if<ClassObject>(&*object);

	ASSERT(class_object->methods.contains(statement.name), "Method name is not found in class definition");
	int method_id = class_object->methods.at(statement.name);

	enter_scope();

	for (auto const& arg_name : statement.arguments)
	{
		int id = define(arg_name);
		emit(OpCode::STORE_LOCAL, id);
	}

	visit(statement.body);

	auto instructions = leave_subroutine_scope();
	int parameter_count = statement.arguments.size();

	auto generator_object = MAKE_OBJECT_VARIANT(GeneratorMethodObject(statement.name, instructions, parameter_count));
	memory->definition_store->set(method_id, move(generator_object));
}

void Compiler::visit(EnumDefinition const& statement)
{
	auto enum_object = MAKE_OBJECT_VARIANT(EnumObject(
		statement.name,
		statement.members
	));

	auto enum_id = define(statement.name);
	memory->definition_store->set(enum_id, move(enum_object));
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

void Compiler::visit(InfixOperatorDefinition const& statement)
{
}

void Compiler::visit(PrefixOperatorDefinition const& statement)
{
}

void Compiler::visit(PostfixOperatorDefinition const& statement)
{
}

// Expression

void Compiler::visit(const Expression_ptr expression)
{
	std::visit(overloaded{
		[&](int expr) { visit(expr); },
		[&](double expr) { visit(expr); },
		[&](std::wstring expr) { visit(expr); },
		[&](bool expr) { visit(expr); },
		[&](ListLiteral const& expr) { visit(expr); },
		[&](TupleLiteral const& expr) { visit(expr); },
		[&](MapLiteral const& expr) { visit(expr); },
		[&](SetLiteral const& expr) { visit(expr); },
		[&](NewObject const& expr) { visit(expr); },
		[&](TernaryCondition const& expr) { visit(expr); },
		[&](EnumMember const& expr) { visit(expr); },
		[&](Identifier const& expr) { visit(expr); },
		[&](Prefix const& expr) { visit(expr); },
		[&](Infix const& expr) { visit(expr); },
		[&](Postfix const& expr) { visit(expr); },
		[&](Call const& expr) { visit(expr); },
		[&](TypePattern const& expr) { visit(expr); },
		[&](Assignment const& expr) { visit(expr); },

		[&](auto)
		{
			FATAL("Never Seen this Statement before!");
		}
		}, *expression);
}

void Compiler::visit(std::vector<Expression_ptr> const& expressions)
{
	for (const auto expr : expressions)
	{
		visit(expr);
	}
}

void Compiler::visit(const int number)
{
	int constant_id = memory->constant_pool->allocate(number);
	emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const double number)
{
	int constant_id = memory->constant_pool->allocate(number);
	emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const std::wstring text)
{
	int constant_id = memory->constant_pool->allocate(text);
	emit(OpCode::PUSH_CONSTANT, constant_id);
}

void Compiler::visit(const bool boolean)
{
	if (boolean)
	{
		emit(OpCode::PUSH_CONSTANT_TRUE);
	}
	else
	{
		emit(OpCode::PUSH_CONSTANT_FALSE);
	}
}

void Compiler::visit(ListLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_LIST, expr.expressions.size());
}

void Compiler::visit(TupleLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_TUPLE, expr.expressions.size());
}

void Compiler::visit(MapLiteral const& expr)
{
	for (const auto [key, value] : expr.pairs)
	{
		visit(key);
		visit(value);
	}

	emit(OpCode::MAKE_MAP, expr.pairs.size());
}

void Compiler::visit(SetLiteral const& expr)
{
	visit(expr.expressions);
	emit(OpCode::MAKE_SET, expr.expressions.size());
}

void Compiler::visit(NewObject const& expr)
{
}

void Compiler::visit(TernaryCondition const& expr)
{
	enter_scope();

	std::visit(overloaded{
		[&](Assignment const& assignment)
		{
			ASSERT(holds_alternative<TypePattern>(*assignment.lhs_expression), "Expected type pattern");
			auto type_pattern = get_if<TypePattern>(&*assignment.lhs_expression);

			ASSERT(holds_alternative<Identifier>(*type_pattern->expression), "Must be an identifier");
			auto identifier = get_if<Identifier>(&*type_pattern->expression);

			visit(assignment.rhs_expression);

			int id = define(identifier->name);
			emit(OpCode::STORE_LOCAL, id);
			emit(OpCode::LOAD_LOCAL, id);
		},

		[&](auto)
		{
			visit(expr.condition);
		}
		}, *expr.condition);

	int	alternative_branch_label = create_label();
	int	exit_branch_label = create_label();

	emit(OpCode::POP_JUMP_IF_FALSE, alternative_branch_label);
	visit(expr.true_expression);
	emit(OpCode::POP_JUMP, exit_branch_label);

	emit(OpCode::LABEL, alternative_branch_label);
	visit(expr.false_expression);

	emit(OpCode::LABEL, exit_branch_label);

	leave_scope();
}

void Compiler::visit(EnumMember const& expr)
{
	auto enum_name = expr.member_chain.front();

	auto scope = scope_stack.top();
	auto enum_id = scope->symbol_table->lookup(enum_name);

	Object_ptr object = memory->definition_store->get(enum_id);
	ASSERT(holds_alternative<EnumObject>(*object), "Expected Enum object");
	auto enum_object = get_if<EnumObject>(&*object);

	wstring enum_string = L"";

	for (const auto member : expr.member_chain)
	{
		enum_string.append(L"::");
		enum_string.append(member);
	}

	enum_string = enum_string.substr(2, enum_string.size());

	auto it = std::find(enum_object->members.begin(), enum_object->members.end(), enum_string);
	ASSERT(it != enum_object->members.end(), "Enum does not contain this member");

	int member_index = std::distance(enum_object->members.begin(), it);
	emit(OpCode::GET_ENUM_MEMBER, enum_id, member_index);
}

void Compiler::visit(TypePattern const& expr)
{
	FATAL("TypePattern cannot be a standalone");
}

void Compiler::visit(Identifier const& expr)
{
	auto scope = scope_stack.top();
	auto id = scope->symbol_table->lookup(expr.name);
	emit(OpCode::LOAD_LOCAL, id);
}

void Compiler::visit(Spread const& expr)
{
}

void Compiler::visit(Call const& expr)
{
	wstring function_name = expr.name;

	int constant_id = memory->constant_pool->allocate(function_name);
	visit(expr.arguments);

	int argument_count = expr.arguments.size();
	emit(OpCode::CALL_FUNCTION, constant_id, argument_count);
}

void Compiler::visit(Prefix const& expr)
{
	visit(expr.operand);

	switch (expr.op->type)
	{
	case WTokenType::BANG:
	{
		emit(OpCode::UNARY_NOT);
		break;
	}
	case WTokenType::MINUS:
	{
		emit(OpCode::UNARY_NEGATIVE);
		break;
	}
	case WTokenType::PLUS:
	{
		emit(OpCode::UNARY_POSITIVE);
		break;
	}
	default: {
		break;
	}
	}
}

void Compiler::visit(Infix const& expr)
{
	visit(expr.right);

	switch (expr.op->type)
	{
	case WTokenType::DOT:
	case WTokenType::QUESTION_DOT:
	{
		// ?
		return;
	}
	}

	visit(expr.left);

	switch (expr.op->type)
	{
	case WTokenType::PLUS:
	{
		emit(OpCode::ADD);
		break;
	}
	case WTokenType::MINUS:
	{
		emit(OpCode::SUBTRACT);
		break;
	}
	case WTokenType::STAR:
	{
		emit(OpCode::MULTIPLY);
		break;
	}
	case WTokenType::DIVISION:
	{
		emit(OpCode::DIVISION);
		break;
	}
	case WTokenType::REMINDER:
	{
		emit(OpCode::REMINDER);
		break;
	}
	case WTokenType::POWER:
	{
		emit(OpCode::POWER);
		break;
	}
	case WTokenType::EQUAL_EQUAL:
	{
		emit(OpCode::EQUAL);
		break;
	}
	case WTokenType::BANG_EQUAL:
	{
		emit(OpCode::NOT_EQUAL);
		break;
	}
	case WTokenType::LESSER_THAN:
	{
		emit(OpCode::LESSER_THAN);
		break;
	}
	case WTokenType::LESSER_THAN_EQUAL:
	{
		emit(OpCode::LESSER_THAN_EQUAL);
		break;
	}
	case WTokenType::GREATER_THAN:
	{
		emit(OpCode::GREATER_THAN);
		break;
	}
	case WTokenType::GREATER_THAN_EQUAL:
	{
		emit(OpCode::GREATER_THAN_EQUAL);
		break;
	}
	case WTokenType::AND:
	{
		emit(OpCode::AND);
		break;
	}
	case WTokenType::OR:
	{
		emit(OpCode::OR);
		break;
	}
	default:
	{
		break;
	}
	}
}

void Compiler::visit(Postfix const& expr)
{
}

void Compiler::visit(Assignment const& statement)
{
	visit(statement.rhs_expression);

	ASSERT(holds_alternative<Identifier>(*statement.lhs_expression), "Must be an identifier");
	auto identifier = get_if<Identifier>(&*statement.lhs_expression);

	auto scope = scope_stack.top();
	int id = scope->symbol_table->lookup(identifier->name);
	emit(OpCode::STORE_LOCAL, id);
}

// Emit

void Compiler::emit(OpCode opcode)
{
	auto scope = scope_stack.top();
	scope->code_section->emit(opcode);
}

void Compiler::emit(OpCode opcode, int operand)
{
	auto scope = scope_stack.top();
	scope->code_section->emit(opcode, operand);
}

void Compiler::emit(OpCode opcode, int operand_1, int operand_2)
{
	auto scope = scope_stack.top();
	scope->code_section->emit(opcode, operand_1, operand_2);
}

// Scope

CScope_ptr Compiler::enter_scope()
{
	if (scope_stack.size() > 0)
	{
		auto scope = scope_stack.top();
		auto enclosing_symbol_table = scope->symbol_table;
		scope_stack.push(make_shared<CScope>(enclosing_symbol_table));
	}
	else
	{
		scope_stack.push(make_shared<CScope>(memory->code_section));
	}

	return scope_stack.top();
}

ByteVector Compiler::leave_scope()
{
	auto old_outer_scope = scope_stack.top();
	scope_stack.pop();

	ByteVector instructions = old_outer_scope->code_section->instructions;

	if (scope_stack.size() > 0)
	{
		auto new_outer_scope = scope_stack.top();

		new_outer_scope->code_section->instructions.insert(
			std::end(new_outer_scope->code_section->instructions),
			std::begin(instructions),
			std::end(instructions)
		);
	}

	return instructions;
}

ByteVector Compiler::leave_subroutine_scope()
{
	auto old_outer_scope = scope_stack.top();
	scope_stack.pop();

	ByteVector instructions = old_outer_scope->code_section->instructions;
	return instructions;
}

// Utils

int Compiler::define(wstring name)
{
	int id = next_id++;

	auto scope = scope_stack.top();
	scope->symbol_table->define(name, id);

	return id;
}

int Compiler::create_label()
{
	int label = next_label++;
	return label;
}