#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include <string>
#include "StatementNodes.h"
#include "ExpressionNodes.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

void print_statement_node(StatementNode_ptr node, int level)
{
	visit(overload{
		[=](Let node) { node.print(level); },
		[=](Const node) { node.print(level); },
		[=](Assignment node) { node.print(level); },
		[=](Branch node) { node.print(level); },
		[=](Loop node) { node.print(level); },
		[=](Break node) { node.print(level); },
		[=](Continue node) { node.print(level); },
		[=](RecordDefinition node) { node.print(level); },
		[=](FunctionDefinition node) { node.print(level); },
		[=](Return node) { node.print(level); },
		[=](ExpressionStatement node) { node.print(level); },
		[=](Import node) { node.print(level); },
		[](std::monostate x) {}
		}, *node.get());
}

void Let::print(int level)
{
	cout << string(level, ' ') << "Let : " << setw(25) << std::left << endl;
	cout << string(level + 4, ' ') << "Public ? : " << this->is_public << endl;
	cout << string(level + 4, ' ') << "Variable name : " << this->name << endl;
	print_type_node(this->type, level + 4);
	print_expression_node(this->expression, level + 4);
}

void Const::print(int level)
{
	cout << string(level, ' ') << "Const : " << setw(25) << std::left << endl;
	cout << string(level + 4, ' ') << "Public ? : " << this->is_public << endl;
	cout << string(level + 4, ' ') << "Variable name : " << this->name << endl;
	print_type_node(this->type, level + 4);
	print_expression_node(this->expression, level + 4);
}

void Assignment::print(int level)
{
	cout << string(level, ' ') << "Assignment : " << setw(25) << std::left << this->name << endl;
	print_expression_node(this->expression, level + 4);
}

void Branch::print(int level)
{
	cout << string(level, ' ') << "Branch : " << setw(25) << std::left << endl;

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
	cout << string(level, ' ') << "Loop : " << setw(25) << std::left << endl;
	for (auto const& statement : this->block)
	{
		print_statement_node(statement, level + 4);
	}
}

void Break::print(int level)
{
	cout << string(level, ' ') << "Break : " << setw(25) << std::left << endl;
}

void Continue::print(int level)
{
	cout << string(level, ' ') << "Continue : " << setw(25) << std::left << endl;
}

void RecordDefinition::print(int level)
{
	cout << string(level, ' ') << "Record Definition : " << setw(25) << std::left << endl;

	cout << string(level + 4, ' ') << "Public ? : " << this->is_public << endl;
	cout << string(level + 4, ' ') << "Record name : " << this->name << endl;

	cout << string(level + 4, ' ') << "Members" << endl;
	for (auto const& pair : this->member_types)
	{
		cout << string(level + 8, ' ') << "Member Name : " << pair.first << endl;
		print_type_node(pair.second, level + 8);
	}
}

void FunctionDefinition::print(int level)
{
	cout << string(level, ' ') << "Function Definition : " << setw(25) << std::left << endl;

	cout << string(level + 4, ' ') << "Public ? : " << this->is_public << endl;
	cout << string(level + 4, ' ') << "Function name : " << this->name << endl;

	cout << string(level + 4, ' ') << "Return Type : " << endl;
	print_type_node(this->return_type.value, level + 4);

	for (auto const& statement : this->body)
	{
		print_statement_node(statement, level + 4);
	}
}

void Return::print(int level)
{
	cout << string(level, ' ') << "Return : " << setw(25) << std::left << endl;
	print_expression_node(this->expression.value, level + 4);
}

void ExpressionStatement::print(int level)
{
	cout << string(level, ' ') << "ExpressionStatement : " << setw(25) << std::left << endl;
	print_expression_node(this->expression, level + 4);
}

void Import::print(int level)
{
	cout << string(level, ' ') << "Import : " << setw(25) << std::left << endl;
	cout << string(level + 4, ' ') << "Path : " << this->path << endl;

	cout << string(level + 4, ' ') << "Goods : " << endl;
	for (auto const& good : goods)
	{
		cout << good << endl;
	}
}