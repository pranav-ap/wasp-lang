#pragma once
#include "pch.h"
#include <iostream>
#include "Token.h"
#include "TokenType.h"

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
	os << token.value << " ";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Token_ptr& token)
{
	os << *token << " ";
	return os;
}