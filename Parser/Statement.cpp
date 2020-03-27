#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include <string>

#include "Statement.h"
#include "Expression.h"
#include "Types.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;
using std::vector;
using std::map;
using std::optional;

void VariableDeclaration::print(int level)
{
	cout << string(level, ' ') << "Variable declaration : " << endl;
	cout << string(level + 4, ' ') << "Variable name : " << this->name << endl;
	cout << string(level + 4, ' ') << "Public : " << std::boolalpha << this->is_public << endl;
	cout << string(level + 4, ' ') << "Mutable : " << std::boolalpha << this->is_mutable << endl;
	this->type->print(level + 4);
	this->expression->print(level + 4);
}

bool VariableDeclaration::is_public_declaration()
{
	return is_public;
}

bool VariableDeclaration::is_mutable_declaration()
{
	return is_mutable;
}

string VariableDeclaration::get_variable_name()
{
	return name;
}

Type_ptr VariableDeclaration::get_type()
{
	return type;
}

Expression_ptr VariableDeclaration::get_expression()
{
	return expression;
}

void Assignment::print(int level)
{
	cout << string(level, ' ') << "Assignment : " << this->name << endl;
	this->expression->print(level + 4);
}

string Assignment::get_variable_name()
{
	return name;
}

Expression_ptr Assignment::get_expression()
{
	return expression;
}

void Branch::print(int level)
{
	cout << string(level, ' ') << "Branch : " << endl;

	cout << string(level + 4, ' ') << "Condition : " << endl;
	this->condition->print(level + 8);

	cout << string(level + 4, ' ') << "Consequence : " << endl;
	for (auto const& statement : *this->consequence)
	{
		statement->print(level + 8);
	}

	cout << string(level + 4, ' ') << "Alternative : " << endl;
	for (auto const& statement : *this->alternative)
	{
		statement->print(level + 8);
	}
}

Expression_ptr Branch::get_condition()
{
	return condition;
}

Block_ptr Branch::get_consequence()
{
	return consequence;
}

Block_ptr Branch::get_alternative()
{
	return alternative;
}

void Loop::print(int level)
{
	cout << string(level, ' ') << "Loop : " << endl;

	for (auto const& statement : *this->block)
	{
		statement->print(level + 4);
	}
}

Block_ptr Loop::get_block()
{
	return block;
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

string Alias::get_alias()
{
	return name;
}

Type_ptr Alias::get_type()
{
	return type;
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

bool RecordDefinition::is_public_declaration()
{
	return is_public;
}

string RecordDefinition::get_name()
{
	return name;
}

map<string, Type_ptr> RecordDefinition::get_member_types()
{
	return member_types;
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
	for (auto const& statement : *this->body)
	{
		statement->print(level + 8);
	}
}

bool FunctionDefinition::is_public_declaration()
{
	return is_public;
}

string FunctionDefinition::get_name()
{
	return name;
}

map<string, Type_ptr> FunctionDefinition::get_arguments()
{
	return arguments;
}

optional<Type_ptr> FunctionDefinition::get_return_type()
{
	return return_type;
}

Block_ptr FunctionDefinition::get_function_body()
{
	return body;
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

optional<Expression_ptr> Return::get_optional_expression()
{
	return expression;
}

void ExpressionStatement::print(int level)
{
	cout << string(level, ' ') << "ExpressionStatement : " << endl;
	this->expression->print(level + 4);
}

Expression_ptr ExpressionStatement::get_expression()
{
	return expression;
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

vector<string> Import::get_goods()
{
	return goods;
}

string Import::get_path()
{
	return path;
}