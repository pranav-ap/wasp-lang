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

bool Token::operator==(const Token& token) const
{
	return (
		this->type == token.type &&
		this->value == token.value &&
		this->line_num == token.line_num &&
		this->column_num == token.column_num
		);
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
	os << token.value << " "; // << " : " << type.type;
	return os;
}

std::ostream& operator<<(std::ostream& os, const Token_ptr& token)
{
	os << token->value << " "; // << " : " << type.type;
	return os;
}