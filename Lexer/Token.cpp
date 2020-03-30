#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "Token.h"
#include "TokenType.h"

using std::cout;
using std::endl;
using std::string;
using std::setw;

string Token::get_value() const
{
	return this->value;
}

WTokenType Token::get_type() const
{
	return this->type;
}

int Token::get_line_num() const
{
	return this->line_num;
}

int Token::get_column_num() const
{
	return this->column_num;
}

void Token::set_line_num(int num)
{
	this->line_num = num;
}

void Token::set_column_num(int num)
{
	this->column_num = num;
}

void Token::print() const
{
	cout << " [ Ln " << setw(3) << std::left << this->line_num
		<< " Col " << setw(3) << std::left << this->column_num << " ] : "
		<< setw(20) << std::left << token_type_to_string(this->type) << " : " << this->value << endl;
}

bool Token::operator==(const Token& token) const
{
	return (
		this->type == token.get_type() &&
		this->value == token.get_value() &&
		this->line_num == token.get_line_num() &&
		this->column_num == token.get_column_num()
		);
}

std::ostream& operator<<(std::ostream& os, const Token& t)
{
	os << t.get_value(); // << " : " << token_type_to_string(t.get_type());
	return os;
}