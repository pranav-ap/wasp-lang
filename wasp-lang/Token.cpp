#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include "Token.h"

using namespace std;

string to_string(TokenType type)
{
	switch (type)
	{

	// Literals

	case TokenType::NumberLiteral:
	{
		return "NumberLiteral";
	}
	case TokenType::StringLiteral:
	{
		return "StringLiteral";
	}
	case TokenType::BooleanLiteral:
	{
		return "BooleanLiteral";
	}

	// Identifiers

	case TokenType::Identifier:
	{
		return "Identifier";
	}
	case TokenType::FunctionIdentifier:
	{
		return "FunctionIdentifier";
	}

	// Punctuations

	case TokenType::OPEN_PARENTHESIS:
	case TokenType::CLOSE_PARENTHESIS:
	case TokenType::OPEN_CURLY_BRACE:
	case TokenType::CLOSE_CURLY_BRACE:
	case TokenType::OPEN_BRACKET:
	case TokenType::CLOSE_BRACKET:
	case TokenType::BACKWARD_SLASH:
	case TokenType::COMMA:
	case TokenType::DOT:
	case TokenType::COLON:
	case TokenType::BAR:
	case TokenType::ARROW:
	case TokenType::PLUS:
	case TokenType::PLUS_EQUAL:
	case TokenType::MINUS:
	case TokenType::MINUS_EQUAL:
	case TokenType::STAR:
	case TokenType::STAR_EQUAL:
	case TokenType::DIVISION:
	case TokenType::DIVISION_EQUAL:
	case TokenType::REMINDER:
	case TokenType::REMINDER_EQUAL:
	case TokenType::POWER:
	case TokenType::POWER_EQUAL:
	case TokenType::EQUAL:
	case TokenType::EQUAL_EQUAL:
	case TokenType::BANG:
	case TokenType::BANG_EQUAL:
	case TokenType::LESSER_THAN:
	case TokenType::LESSER_THAN_EQUAL:
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_THAN_EQUAL:
	{
		return "Punctuation";
	}

	// Keywords

	case TokenType::IF:
	case TokenType::ELSE:
	case TokenType::AND:
	case TokenType::OR:
	case TokenType::LET:
	case TokenType::CONST:
	case TokenType::LOOP:
	case TokenType::FOR:
	case TokenType::BREAK:
	case TokenType::CONTINUE:
	case TokenType::FN:
	case TokenType::RETURN:
	case TokenType::NUM:
	case TokenType::STR:
	case TokenType::BOOL:
	case TokenType::ENUM:
	case TokenType::TYPE:
	case TokenType::OPT:
	case TokenType::TRUE:
	case TokenType::FALSE:
	case TokenType::IMPORT:
	case TokenType::FROM:
	case TokenType::PUB:
	case TokenType::PURE:
	{
		return "Keyword";
	}

	// Other

	case TokenType::EOL:
	{
		return "EOL";
	}
	case TokenType::UNKNOWN:
	{
		return "UNKNOWN";
	}
	}

	return "";
}

string Token::get_value() const
{
	return this->value;
}

TokenType Token::get_type() const
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
	cout << " [ Ln " << setw(3) << std::left << this->line_num;
	cout << " Col " << setw(3) << std::left << this->column_num << " ] : ";
	cout << setw(13) << std::left << to_string(this->type) << " : ";
	cout << setw(20) << std::left << this->value << endl;
}

bool Token::operator==(const Token& token) const
{
	return this->type == token.get_type() && this->value == token.get_value() && 
		this->line_num == token.get_line_num() && this->column_num == token.get_column_num();
}
