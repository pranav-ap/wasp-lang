#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include <type_traits>
#include "StatementNodes.h"
#include "ExpressionNodes.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

void print_statement_node(StatementNode_ptr node, int level)
{
	visit([level](auto&& n) {
		using T = std::decay_t<decltype(n)>;

		if constexpr (std::is_same_v<T, VariableDeclaration>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Assignment>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Branch>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Loop>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Break>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Continue>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Return>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Alias>)
			n.print(level);
		else if constexpr (std::is_same_v<T, RecordDefinition>)
			n.print(level);
		else if constexpr (std::is_same_v<T, FunctionDefinition>)
			n.print(level);
		else if constexpr (std::is_same_v<T, ExpressionStatement>)
			n.print(level);
		else if constexpr (std::is_same_v<T, Import>)
			n.print(level);
		}, *node.get());
}

void VariableDeclaration::print(int level)
{
	cout << string(level, ' ') << "Variable declaration : " << endl;
	cout << string(level + 4, ' ') << "Variable name : " << this->name << endl;
	cout << string(level + 4, ' ') << "Public : " << std::boolalpha << this->is_public << endl;
	cout << string(level + 4, ' ') << "Mutable : " << std::boolalpha << this->is_mutable << endl;
	print_type_node(this->type, level + 4);
	print_expression_node(this->expression, level + 4);
}

void Assignment::print(int level)
{
	cout << string(level, ' ') << "Assignment : " << this->name << endl;
	print_expression_node(this->expression, level + 4);
}

void Branch::print(int level)
{
	cout << string(level, ' ') << "Branch : " << endl;

	cout << string(level + 4, ' ') << "Condition : " << endl;
	print_expression_node(this->condition, level + 8);

	cout << string(level + 4, ' ') << "Consequence : " << endl;
	for (auto const& statement : this->consequence)
	{
		print_statement_node(statement, level + 8);
	}

	cout << string(level + 4, ' ') << "Alternative : " << endl;
	for (auto const& statement : this->alternative)
	{
		print_statement_node(statement, level + 8);
	}
}

void Loop::print(int level)
{
	cout << string(level, ' ') << "Loop : " << endl;

	for (auto const& statement : this->block)
	{
		print_statement_node(statement, level + 4);
	}
}

void Break::print(int level)
{
	cout << string(level, ' ') << "Break" << endl;
}

void Continue::print(int level)
{
	cout << string(level, ' ') << "Continue" << endl;
}

void Alias::print(int level)
{
	cout << string(level, ' ') << "Alias : " << this->name << endl;
	print_type_node(this->type, level + 4);
}

void RecordDefinition::print(int level)
{
	cout << string(level, ' ') << "Record Definition : " << endl;

	cout << string(level + 4, ' ') << "Public : " << std::boolalpha << this->is_public << endl;
	cout << string(level + 4, ' ') << "Record name : " << this->name << endl;

	cout << string(level + 4, ' ') << "Members : " << endl;
	for (auto const& pair : this->member_types)
	{
		cout << string(level + 8, ' ') << "Name : " << pair.first << endl;
		print_type_node(pair.second, level + 8);
		cout << endl;
	}
}

void FunctionDefinition::print(int level)
{
	cout << string(level, ' ') << "Function Definition : " << endl;

	cout << string(level + 4, ' ') << "Public : " << std::boolalpha << this->is_public << endl;
	cout << string(level + 4, ' ') << "Function name : " << this->name << endl;

	if (this->arguments.size() > 0)
	{
		cout << string(level + 4, ' ') << "Arguments : " << endl;

		for (auto const& pair : this->arguments)
		{
			cout << string(level + 8, ' ') << "Name : " << pair.first << endl;
			print_type_node(pair.second, level + 8);
		}
	}
	else
		cout << string(level + 4, ' ') << "Arguments : None" << endl;

	if (this->return_type)
	{
		cout << string(level + 4, ' ') << "Return Type : " << endl;
		print_type_node(this->return_type.value(), level + 8);
	}
	else
		cout << string(level + 4, ' ') << "Return Type : None" << endl;

	cout << string(level + 4, ' ') << "Body : " << endl;
	for (auto const& statement : this->body)
	{
		print_statement_node(statement, level + 8);
	}
}

void Return::print(int level)
{
	cout << string(level, ' ') << "Return : " << endl;
	print_expression_node(this->expression.value(), level + 4);
}

void ExpressionStatement::print(int level)
{
	cout << string(level, ' ') << "ExpressionStatement : " << endl;
	print_expression_node(this->expression, level + 4);
}

void Import::print(int level)
{
	cout << string(level, ' ') << "Import : " << endl;
	cout << string(level + 4, ' ') << "Path : " << this->path << endl;

	cout << string(level + 4, ' ') << "Goods : " << endl;
	for (auto const& good : goods)
	{
		cout << good << endl;
	}
}