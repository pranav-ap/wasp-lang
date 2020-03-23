#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include "Token.h"
#include "TokenType.h"

using std::cout;
using std::endl;
using std::string;
using std::setw;

string to_string(WTokenType type)
{
	switch (type)
	{
		// Literals

	case WTokenType::NumberLiteral:
	{
		return "Number Literal";
	}
	case WTokenType::StringLiteral:
	{
		return "String Literal";
	}

	// Identifiers

	case WTokenType::Identifier:
	{
		return "Identifier";
	}
	case WTokenType::FunctionIdentifier:
	{
		return "Function Identifier";
	}

	// Punctuations

	case WTokenType::OPEN_PARENTHESIS:
	case WTokenType::CLOSE_PARENTHESIS:
	case WTokenType::OPEN_BRACKET:
	case WTokenType::CLOSE_BRACKET:
	case WTokenType::OPEN_CURLY_BRACE:
	case WTokenType::CLOSE_CURLY_BRACE:
	case WTokenType::BACKWARD_SLASH:
	case WTokenType::COMMA:
	case WTokenType::DOT:
	case WTokenType::COLON:
	case WTokenType::BAR:
	case WTokenType::ARROW:
	case WTokenType::PLUS:
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS:
	case WTokenType::MINUS_EQUAL:
	case WTokenType::STAR:
	case WTokenType::STAR_EQUAL:
	case WTokenType::DIVISION:
	case WTokenType::DIVISION_EQUAL:
	case WTokenType::REMINDER:
	case WTokenType::REMINDER_EQUAL:
	case WTokenType::POWER:
	case WTokenType::POWER_EQUAL:
	case WTokenType::EQUAL:
	case WTokenType::EQUAL_EQUAL:
	case WTokenType::BANG:
	case WTokenType::BANG_EQUAL:
	case WTokenType::LESSER_THAN:
	case WTokenType::LESSER_THAN_EQUAL:
	case WTokenType::GREATER_THAN:
	case WTokenType::GREATER_THAN_EQUAL:
	{
		return "Punctuation";
	}

	// Keywords

	case WTokenType::IF:
	case WTokenType::ELSE:
	case WTokenType::AND:
	case WTokenType::OR:
	case WTokenType::LET:
	case WTokenType::CONST_KEYWORD:
	case WTokenType::LOOP:
	case WTokenType::FOR:
	case WTokenType::BREAK:
	case WTokenType::CONTINUE:
	case WTokenType::FN:
	case WTokenType::RETURN:
	case WTokenType::NUM:
	case WTokenType::STR:
	case WTokenType::BOOL:
	case WTokenType::ENUM:
	case WTokenType::TYPE:
	case WTokenType::OPT:
	case WTokenType::TRUE_KEYWORD:
	case WTokenType::FALSE_KEYWORD:
	case WTokenType::IMPORT:
	case WTokenType::FROM:
	case WTokenType::PUB:
	{
		return "Keyword";
	}

	// Other

	case WTokenType::EOL:
	{
		return "EOL";
	}
	case WTokenType::UNKNOWN:
	{
		return "UNKNOWN";
	}
	default:
		return "";
	}
}

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

void Token::print() const
{
	cout << " [ Ln " << setw(3) << std::left << this->line_num
		<< " Col " << setw(3) << std::left << this->column_num << " ] : "
		<< setw(20) << std::left << to_string(this->type) << " : " << this->value << endl;
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