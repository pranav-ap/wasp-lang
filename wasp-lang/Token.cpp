#pragma once
#include "Token.h"
#include <memory>

using namespace std;

// << Overloading

ostream& operator<< (ostream& out, unique_ptr<TokenType> const& token) {
	out << " - Ln : " << token->line_num << "Col : " << token->column_num;
	return out;
}

ostream& operator<< (ostream& out, unique_ptr<NumberLiteral> const& token) {
	out << token->value << " - Ln : " << token->line_num << "Col : " << token->column_num;
	return out;
}

ostream &operator<<(ostream &out, unique_ptr<StringLiteral> const &token)
{
	out << token->value << " - Ln : " << token->line_num << "Col : " << token->column_num;
	return out;
}

ostream &operator<<(ostream &out, unique_ptr<BooleanLiteral> const &token)
{
	out << token->value << " - Ln : " << token->line_num << "Col : " << token->column_num;
	return out;
}

ostream &operator<<(ostream &out, unique_ptr<Keyword> const &token)
{
	switch (token->value) {
	case KeywordType::IF:
	{
		out << "Keyword : IF - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::ELSE:
	{
		out << "Keyword : ELSE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::AND:
	{
		out << "Keyword : AND - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::OR:
	{
		out << "Keyword : OR - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::LET:
	{
		out << "Keyword : LET - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::CONST:
	{
		out << "Keyword : CONST - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::LOOP:
	{
		out << "Keyword : LOOP - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::FOR:
	{
		out << "Keyword : FOR - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::BREAK:
	{
		out << "Keyword : BREAK - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::CONTINUE:
	{
		out << "Keyword : CONTINUE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::FN:
	{
		out << "Keyword : FN - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::RETURN:
	{
		out << "Keyword : RETURN - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::NUM:
	{
		out << "Keyword : NUM - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::STR:
	{
		out << "Keyword : STR - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::BOOL:
	{
		out << "Keyword : BOOL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::ENUM:
	{
		out << "Keyword : ENUM - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::TYPE:
	{
		out << "Keyword : TYPE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::OPT:
	{
		out << "Keyword : OPT - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::TRUE:
	{
		out << "Keyword : TRUE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::FALSE:
	{
		out << "Keyword : FALSE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::IMPORT:
	{
		out << "Keyword : IMPORT - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::FROM:
	{
		out << "Keyword : FROM - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::PUB:
	{
		out << "Keyword : PUB - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case KeywordType::PURE:
	{
		out << "Keyword : PURE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	default:
		break;
	}

	return out;
}

ostream &operator<<(ostream &out, unique_ptr<Punctuation> const &token)
{
	switch (token->value)
	{
	case PunctuationType::OPEN_PARENTHESIS:
	{
		out << "Punctuation : OPEN_PARENTHESIS - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::CLOSE_PARENTHESIS:
	{
		out << "Punctuation : CLOSE_PARENTHESIS - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::OPEN_CURLY_BRACE:
	{
		out << "Punctuation : OPEN_CURLY_BRACE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::CLOSE_CURLY_BRACE:
	{
		out << "Punctuation : CLOSE_CURLY_BRACE - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::OPEN_BRACKET:
	{
		out << "Punctuation : OPEN_BRACKET - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::CLOSE_BRACKET:
	{
		out << "Punctuation : CLOSE_BRACKET - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::BACKWARD_SLASH:
	{
		out << "Punctuation : BACKWARD_SLASH - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::SEMICOLON:
	{
		out << "Punctuation : SEMICOLON - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::COMMA:
	{
		out << "Punctuation : COMMA - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::COLON:
	{
		out << "Punctuation : COLON - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::BAR:
	{
		out << "Punctuation : BAR - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::ARROW:
	{
		out << "Punctuation : ARROW - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::PLUS:
	{
		out << "Punctuation : PLUS - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::PLUS_EQUAL:
	{
		out << "Punctuation : PLUS_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::MINUS:
	{
		out << "Punctuation : MINUS - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::MINUS_EQUAL:
	{
		out << "Punctuation : MINUS_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::STAR:
	{
		out << "Punctuation : STAR - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::STAR_EQUAL:
	{
		out << "Punctuation : STAR_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::DIVISION:
	{
		out << "Punctuation : DIVISION - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::DIVISION_EQUAL:
	{
		out << "Punctuation : DIVISION_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::REMINDER:
	{
		out << "Punctuation : REMINDER - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::REMINDER_EQUAL:
	{
		out << "Punctuation : REMINDER_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::POWER:
	{
		out << "Punctuation : POWER - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::POWER_EQUAL:
	{
		out << "Punctuation : POWER_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::EQUAL:
	{
		out << "Punctuation : EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::EQUAL_EQUAL:
	{
		out << "Punctuation : EQUAL_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::BANG:
	{
		out << "Punctuation : BANG - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::BANG_EQUAL:
	{
		out << "Punctuation : BANG_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::LESSER_THAN:
	{
		out << "Punctuation : LESSER_THAN - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::LESSER_THAN_EQUAL:
	{
		out << "Punctuation : LESSER_THAN_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::GREATER_THAN:
	{
		out << "Punctuation : GREATER_THAN - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	case PunctuationType::GREATER_THAN_EQUAL:
	{
		out << "Punctuation : GREATER_THAN_EQUAL - " << " - Ln : " << token->line_num << "Col : " << token->column_num;
		break;
	}
	default:
		break;
	}

	return out;
}

ostream &operator<<(ostream &out, unique_ptr<Unknown> const &token)
{
	out << token->value << " - Ln : " << token->line_num << "Col : " << token->column_num;
	return out;
}