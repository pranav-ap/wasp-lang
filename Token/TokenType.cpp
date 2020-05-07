#pragma once
#include "pch.h"
#include "TokenType.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const WTokenType type)
{
	switch (type)
	{
		// Literals

	case WTokenType::NumberLiteral:
	{
		os << "Number Literal";
		break;
	}
	case WTokenType::StringLiteral:
	{
		os << "String Literal";
		break;
	}

	// Identifiers

	case WTokenType::Identifier:
	{
		os << "Identifier";
		break;
	}
	case WTokenType::CallableIdentifier:
	{
		os << "Callable Identifier";
		break;
	}

	// Punctuations

	case WTokenType::OPEN_PARENTHESIS:
	case WTokenType::CLOSE_PARENTHESIS:
	case WTokenType::OPEN_SQUARE_BRACKET:
	case WTokenType::CLOSE_SQUARE_BRACKET:
	case WTokenType::OPEN_CURLY_BRACE:
	case WTokenType::CLOSE_CURLY_BRACE:
	case WTokenType::OPEN_TUPLE_PARENTHESIS:
	case WTokenType::BACKWARD_SLASH:
	case WTokenType::ARROW:
	case WTokenType::COMMA:
	case WTokenType::DOT:
	case WTokenType::COLON:
	case WTokenType::COLON_COLON:
	case WTokenType::PLUS:
	case WTokenType::UNARY_PLUS:
	case WTokenType::PLUS_EQUAL:
	case WTokenType::MINUS:
	case WTokenType::UNARY_MINUS:
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
		os << "Punctuation";
		break;
	}

	// Keywords

	case WTokenType::IF:
	case WTokenType::ELSE:
	case WTokenType::AND:
	case WTokenType::OR:
	case WTokenType::LET:
	case WTokenType::CONST_KEYWORD:
	case WTokenType::WHILE:
	case WTokenType::FOR:
	case WTokenType::BREAK:
	case WTokenType::CONTINUE:
	case WTokenType::FN:
	case WTokenType::RETURN:
	case WTokenType::GEN:
	case WTokenType::YIELD_KEYWORD:
	case WTokenType::NUM:
	case WTokenType::STR:
	case WTokenType::BOOL:
	case WTokenType::ENUM:
	case WTokenType::TYPE:
	case WTokenType::ANY:
	case WTokenType::OPT:
	case WTokenType::NONE:
	case WTokenType::TRUE_KEYWORD:
	case WTokenType::FALSE_KEYWORD:
	case WTokenType::IMPORT:
	case WTokenType::FROM:
	case WTokenType::PASS:
	case WTokenType::PUB:
	{
		os << "Keyword";
		break;
	}

	// Other

	case WTokenType::EOL:
	{
		os << "EOL";
		break;
	}
	case WTokenType::SPACE:
	{
		os << "SPACE";
		break;
	}
	case WTokenType::COMMENT:
	{
		os << "COMMENT";
		break;
	}
	case WTokenType::UNKNOWN:
	{
		os << "UNKNOWN";
		break;
	}
	default:
		os << "";
	}

	return os;
}