#pragma once
#include "pch.h"
#include "ASTVisualizer.h"
#include "Assertion.h"
#include <string>
#include <fstream>

#define ARROW L" -> "
#define NEW_LINE L" \n "
#define COLON L";"

using std::to_wstring;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void ASTVisualizer::save(int id, std::wstring label)
{
	std::wstring id_str = quote(to_wstring(id));
	label = quote(label);

	content.append(id_str + L"   [label=" + label + L"]" + COLON + NEW_LINE);
}

void ASTVisualizer::save(int child, int parent, std::wstring label)
{
	std::wstring child_str = quote(to_wstring(child));
	std::wstring parent_str = quote(to_wstring(parent));
	label = quote(label);

	content.append(child_str + L"   [label=" + label + L"]" + COLON + NEW_LINE);
	content.append(parent_str + ARROW + child_str + COLON + NEW_LINE);
}

std::wstring ASTVisualizer::quote(std::wstring part)
{
	return L"\"" + part + L"\"";
}

// Statement

void ASTVisualizer::visit(const Statement_ptr statement, int parent_id)
{
	std::visit(overloaded{
		[&](IfBranch const& stat) { visit(stat, parent_id); },
		[&](ElseBranch const& stat) { visit(stat, parent_id); },
		[&](WhileLoop const& stat) { visit(stat, parent_id); },
		[&](ForInLoop const& stat) { visit(stat, parent_id); },
		[&](Break const& stat) { visit(stat, parent_id); },
		[&](Continue const& stat) { visit(stat, parent_id); },
		[&](Return const& stat) { visit(stat, parent_id); },
		[&](YieldStatement const& stat) { visit(stat, parent_id); },
		[&](VariableDefinition const& stat) { visit(stat, parent_id); },
		[&](ClassDefinition const& stat) { visit(stat, parent_id); },
		[&](InterfaceDefinition const& stat) { visit(stat, parent_id); },
		[&](AliasDefinition const& stat) { visit(stat, parent_id); },
		[&](FunctionDefinition const& stat) { visit(stat, parent_id); },
		[&](GeneratorDefinition const& stat) { visit(stat, parent_id); },
		[&](EnumDefinition const& stat) { visit(stat, parent_id); },
		[&](FunctionMethodDefinition const& stat) { visit(stat, parent_id); },
		[&](GeneratorMethodDefinition const& stat) { visit(stat, parent_id); },
		[&](ExpressionStatement const& stat) { visit(stat, parent_id); },
		[&](Assert const& stat) { visit(stat, parent_id); },
		[&](Implore const& stat) { visit(stat, parent_id); },
		[&](Swear const& stat) { visit(stat, parent_id); },
		[&](Module const& stat) { visit(stat, parent_id); },

		[](auto) { FATAL("Never seen this Statement before! So I cannot print it!"); }
		}, *statement);
}

void ASTVisualizer::visit(std::vector<Statement_ptr> const& statements, int parent_id)
{
	for (const auto statement : statements)
	{
		visit(statement, parent_id);
	}
}

void ASTVisualizer::visit(IfBranch const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"If");

	visit(statement.test, id);
	visit(statement.body, id);

	if (statement.alternative.has_value())
	{
		visit(statement.alternative.value(), id);
	}
}

void ASTVisualizer::visit(ElseBranch const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"else");
	visit(statement.body, id);
}

void ASTVisualizer::visit(WhileLoop const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"while");

	visit(statement.pattern, id);
	visit(statement.block, id);
}

void ASTVisualizer::visit(ForInLoop const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"for");

	visit(statement.pattern, id);
	visit(statement.block, id);
}

void ASTVisualizer::visit(Break const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"break");
}

void ASTVisualizer::visit(Continue const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"continue");
}

void ASTVisualizer::visit(Return const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"return");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value(), id);
	}
}

void ASTVisualizer::visit(YieldStatement const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"yield");

	if (statement.expression.has_value())
	{
		visit(statement.expression.value(), id);
	}
}

void ASTVisualizer::visit(VariableDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"VariableDefinition");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(ClassDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"UDT Definition : " + statement.name);

	for (const auto member : statement.member_types)
	{
		const int member_id = id_counter++;
		save(member_id, id, member.first);
		visit(member.second, member_id);
	}
}

void ASTVisualizer::visit(InterfaceDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Interface Definition");
	visit(statement.name, id);

	for (const auto member : statement.member_types)
	{
		const int member_id = id_counter++;
		save(member_id, id, member.first);
		visit(member.second, member_id);
	}
}

void ASTVisualizer::visit(AliasDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Alias Definition");
	visit(statement.name, id);
	visit(statement.type, id);
}

void ASTVisualizer::visit(FunctionDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Function Definition : " + statement.name);

	if (statement.arguments.size() > 0)
	{
		visit(statement.arguments, id);
	}

	visit(statement.type, id);

	if (statement.block.size() > 0)
	{
		visit(statement.block, id);
	}
}

void ASTVisualizer::visit(GeneratorDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Generator Definition : " + statement.name);

	if (statement.arguments.size() > 0)
	{
		visit(statement.arguments, id);
	}

	visit(statement.type, id);

	if (statement.block.size() > 0)
	{
		visit(statement.block, id);
	}
}

void ASTVisualizer::visit(EnumDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"EnumDefinition");
}

void ASTVisualizer::visit(FunctionMethodDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Function Method Definition " + statement.type_name + L"::" + statement.name);

	if (statement.arguments.size() > 0)
	{
		visit(statement.arguments, id);
	}

	visit(statement.type, id);

	if (statement.body.size() > 0)
	{
		visit(statement.body, id);
	}
}

void ASTVisualizer::visit(GeneratorMethodDefinition const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Generator Method Definition " + statement.type_name + L"::" + statement.name);

	if (statement.arguments.size() > 0)
	{
		visit(statement.arguments, id);
	}

	visit(statement.type, id);

	if (statement.body.size() > 0)
	{
		visit(statement.body, id);
	}
}

void ASTVisualizer::visit(ExpressionStatement const& statement, int parent_id)
{
	visit(statement.expression, parent_id);
}

void ASTVisualizer::visit(Assert const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"assert");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(Implore const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"implore");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(Swear const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"swear");
	visit(statement.expression, id);
}

void ASTVisualizer::visit(Module const& statement, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Module " + statement.name);
	visit(statement.statements, id);
}

// Expression

void ASTVisualizer::visit(const Expression_ptr expression, int parent_id)
{
	std::visit(overloaded{
		[&](int const expr) { visit(expr, parent_id); },
		[&](double const expr) { visit(expr, parent_id); },
		[&](bool const expr) { visit(expr, parent_id); },
		[&](std::wstring const& expr) { visit(expr, parent_id); },
		[&](ListLiteral const& expr) { visit(expr, parent_id); },
		[&](TupleLiteral const& expr) { visit(expr, parent_id); },
		[&](SetLiteral const& expr) { visit(expr, parent_id); },
		[&](MapLiteral const& expr) { visit(expr, parent_id); },
		[&](NewObject const& expr) { visit(expr, parent_id); },
		[&](EnumMember const& expr) { visit(expr, parent_id); },
		[&](Identifier const& expr) { visit(expr, parent_id); },
		[&](Call const& expr) { visit(expr, parent_id); },
		[&](Prefix const& expr) { visit(expr, parent_id); },
		[&](Infix const& expr) { visit(expr, parent_id); },
		[&](Postfix const& expr) { visit(expr, parent_id); },
		[&](TypePattern const& expr) { visit(expr, parent_id); },
		[&](Assignment const& expr) { visit(expr, parent_id); },
		[&](SpreadExpression const& expr) { visit(expr, parent_id); },
		[&](TernaryCondition const& expr) { visit(expr, parent_id); },

		[](auto) { FATAL("Never seen this Expression before! So I cannot print it!"); }
		}, *expression);
}

void ASTVisualizer::visit(std::vector<Expression_ptr> const& expressions, int parent_id)
{
	for (const auto expression : expressions)
	{
		visit(expression, parent_id);
	}
}

void ASTVisualizer::visit(int const expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, to_wstring(expr));
}

void ASTVisualizer::visit(double const expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, to_wstring(expr));
}

void ASTVisualizer::visit(bool const expr, int parent_id)
{
	const int id = id_counter++;
	std::wstring term = expr ? L"true" : L"false";
	save(id, parent_id, term);
}

void ASTVisualizer::visit(std::wstring const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr);
}

void ASTVisualizer::visit(ListLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"List Literal");
	visit(expr.expressions, id);
}

void ASTVisualizer::visit(TupleLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Tuple Literal");
	visit(expr.expressions, id);
}

void ASTVisualizer::visit(SetLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Set Literal");
	visit(expr.expressions, id);
}

void ASTVisualizer::visit(MapLiteral const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Map Literal");
}

void ASTVisualizer::visit(NewObject const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.UDT_name);
	visit(expr.expressions, id);
}

void ASTVisualizer::visit(EnumMember const& expr, int parent_id)
{
	std::wstring member_chain = L"";

	for (const auto member : expr.member_chain)
	{
		member_chain.append(member + L"::");
	}

	member_chain = member_chain.substr(0, member_chain.size() - 2);

	const int id = id_counter++;
	save(id, parent_id, member_chain);
}

void ASTVisualizer::visit(Identifier const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.name);
}

void ASTVisualizer::visit(Call const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"call " + expr.name);
	visit(expr.arguments, id);
}

void ASTVisualizer::visit(Prefix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);
	visit(expr.operand, id);
}

void ASTVisualizer::visit(Infix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);

	visit(expr.left, id);
	visit(expr.right, id);
}

void ASTVisualizer::visit(Postfix const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, expr.op->value);
	visit(expr.operand, id);
}

void ASTVisualizer::visit(TypePattern const& expr, int parent_id)
{
	const int colon_id = id_counter++;
	save(colon_id, parent_id, L":");

	visit(expr.expression, colon_id);
	visit(expr.type, colon_id);
}

void ASTVisualizer::visit(SpreadExpression const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"...");

	visit(expr.expression, id);
}

void ASTVisualizer::visit(Assignment const& expr, int parent_id)
{
	const int equal_id = id_counter++;
	save(equal_id, parent_id, L"=");

	visit(expr.lhs_expression, equal_id);
	visit(expr.rhs_expression, equal_id);
}

void ASTVisualizer::visit(TernaryCondition const& expr, int parent_id)
{
	const int if_id = id_counter++;
	save(if_id, parent_id, L"if");

	visit(expr.condition, if_id);
	visit(expr.true_expression, if_id);

	if (expr.false_expression.has_value())
	{
		const int else_id = id_counter++;
		save(else_id, if_id, L"else");
		visit(expr.false_expression.value(), else_id);
	}
}

// Type

void ASTVisualizer::visit(const Type_ptr type, int parent_id)
{
	std::visit(overloaded{
		   [&](AnyType const& ty) { visit(ty, parent_id); },
		   [&](IntLiteralType const& ty) { visit(ty, parent_id); },
		   [&](FloatLiteralType const& ty) { visit(ty, parent_id); },
		   [&](StringLiteralType const& ty) { visit(ty, parent_id); },
		   [&](BooleanLiteralType const& ty) { visit(ty, parent_id); },
		   [&](IntType const& ty) { visit(ty, parent_id); },
		   [&](FloatType const& ty) { visit(ty, parent_id); },
		   [&](StringType const& ty) { visit(ty, parent_id); },
		   [&](BooleanType const& ty) { visit(ty, parent_id); },
		   [&](ListType const& ty) { visit(ty, parent_id); },
		   [&](TupleType const& ty) { visit(ty, parent_id); },
		   [&](SetType const& ty) { visit(ty, parent_id); },
		   [&](UDTType const& ty) { visit(ty, parent_id); },
		   [&](MapType const& ty) { visit(ty, parent_id); },
		   [&](EnumType const& ty) { visit(ty, parent_id); },
		   [&](VariantType const& ty) { visit(ty, parent_id); },
		   [&](NoneType const& ty) { visit(ty, parent_id); },
		   [&](FunctionType const& ty) { visit(ty, parent_id); },
		   [&](GeneratorType const& ty) { visit(ty, parent_id); },

		   [](auto) {FATAL("Never seen this Type before! So I cannot print it!"); }
		}, *type);
}

void ASTVisualizer::visit(std::vector<Type_ptr> const& types, int parent_id)
{
	for (const auto type : types)
	{
		visit(type, parent_id);
	}
}

void ASTVisualizer::visit(AnyType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"any");
}

void ASTVisualizer::visit(IntLiteralType const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Int Literal Type");
	visit(expr.value, id);
}

void ASTVisualizer::visit(FloatLiteralType const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Float Literal Type");
	visit(expr.value, id);
}

void ASTVisualizer::visit(StringLiteralType const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"String Literal Type");
	visit(expr.value, id);
}

void ASTVisualizer::visit(BooleanLiteralType const& expr, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Boolean Literal Type");
	std::wstring value = expr.value ? L"true" : L"false";
	visit(value, id);
}

void ASTVisualizer::visit(IntType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"int");
}

void ASTVisualizer::visit(FloatType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"float");
}

void ASTVisualizer::visit(StringType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"string");
}

void ASTVisualizer::visit(BooleanType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"bool");
}

void ASTVisualizer::visit(ListType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"List Type");
	visit(type.element_type, id);
}

void ASTVisualizer::visit(TupleType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Tuple Type");
	visit(type.element_types, id);
}

void ASTVisualizer::visit(SetType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Set Type");
	visit(type.element_types, id);
}

void ASTVisualizer::visit(UDTType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"UDT Type");
	visit(type.name, id);
}

void ASTVisualizer::visit(MapType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Map Type");
	visit(type.key_type, id);
	visit(type.value_type, id);
}

void ASTVisualizer::visit(EnumType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Enum Type");
	visit(type.enum_name, id);
}

void ASTVisualizer::visit(VariantType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Variant Type");
	visit(type.types, id);
}

void ASTVisualizer::visit(NoneType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"None Type");
}

void ASTVisualizer::visit(FunctionType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Function Type");

	if (type.input_types.size() > 0)
	{
		const int input_types_id = id_counter++;
		save(input_types_id, id, L"Input Types");
		visit(type.input_types, input_types_id);
	}

	if (type.return_type.has_value())
	{
		const int return_type_id = id_counter++;
		save(return_type_id, id, L"Return Type");
		visit(type.return_type.value(), return_type_id);
	}
}

void ASTVisualizer::visit(GeneratorType const& type, int parent_id)
{
	const int id = id_counter++;
	save(id, parent_id, L"Generator Type");

	if (type.input_types.size() > 0)
	{
		const int input_types_id = id_counter++;
		save(input_types_id, id, L"Input Types");
		visit(type.input_types, input_types_id);
	}

	if (type.return_type.has_value())
	{
		visit(type.return_type.value(), id);
	}
}

// Generate

void ASTVisualizer::generate_dot_file(File_ptr mod)
{
	// reset

	content = L"";
	id_counter = 0;

	// create content

	content.append(L"digraph G { \n ");

	const int id = id_counter++;
	save(id, L"root");

	visit(mod->statements, id);

	content.append(L"}");

	// write to file

	std::string path = "../examples/module.dot";
	std::wofstream ofs(path, std::wofstream::out);
	ofs << content;
	ofs.close();
}