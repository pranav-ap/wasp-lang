#pragma once
#include "pch.h"
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