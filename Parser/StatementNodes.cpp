#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include <string>

#include "StatementNodes.h"
#include "ExpressionNodes.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;

void VariableDeclaration::print(int level)
{
	cout << string(level, ' ') << "Variable declaration : " << endl;
	cout << string(level + 4, ' ') << "Variable name : " << this->name << endl;
	cout << string(level + 4, ' ') << "Public : " << std::boolalpha << this->is_public << endl;
	cout << string(level + 4, ' ') << "Mutable : " << std::boolalpha << this->is_mutable << endl;
	this->type->print(level + 4);
	this->expression->print(level + 4);
}

void Assignment::print(int level)
{
	cout << string(level, ' ') << "Assignment : " << this->name << endl;
	this->expression->print(level + 4);
}

void Branch::print(int level)
{
	cout << string(level, ' ') << "Branch : " << endl;

	cout << string(level + 4, ' ') << "Condition : " << endl;
	this->condition->print(level + 8);

	cout << string(level + 4, ' ') << "Consequence : " << endl;
	for (auto const& statement : this->consequence)
	{
		statement->print(level + 8);
	}

	cout << string(level + 4, ' ') << "Alternative : " << endl;
	for (auto const& statement : this->alternative)
	{
		statement->print(level + 8);
	}
}

void Loop::print(int level)
{
	cout << string(level, ' ') << "Loop : " << endl;

	for (auto const& statement : this->block)
	{
		statement->print(level + 4);
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
	this->type->print(level + 4);
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
		pair.second->print(level + 8);
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
			pair.second->print(level + 8);
		}
	}
	else
		cout << string(level + 4, ' ') << "Arguments : None" << endl;

	if (this->return_type)
	{
		cout << string(level + 4, ' ') << "Return Type : " << endl;
		this->return_type.value()->print(level + 8);
	}
	else
		cout << string(level + 4, ' ') << "Return Type : None" << endl;

	cout << string(level + 4, ' ') << "Body : " << endl;
	for (auto const& statement : this->body)
	{
		statement->print(level + 8);
	}
}

void Return::print(int level)
{
	cout << string(level, ' ') << "Return : " << endl;

	if (this->expression)
	{
		cout << string(level + 4, ' ') << "Return Type : " << endl;
		this->expression.value()->print(level + 4);
	}
	else
		cout << string(level + 4, ' ') << "Return Type : None" << endl;
}

void ExpressionStatement::print(int level)
{
	cout << string(level, ' ') << "ExpressionStatement : " << endl;
	this->expression->print(level + 4);
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