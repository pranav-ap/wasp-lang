#pragma once
#include "Token.h"
#include <memory>

using namespace std;

// Print out Tokens

void NumberLiteral::print()
{
	cout << "NumberLiteral : " << this->value << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
}

void StringLiteral::print()
{
	cout << "StringLiteral : " << this->value << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
}

void BooleanLiteral::print()
{
	cout << "BooleanLiteral : " << this->value << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
}

void Identifier::print()
{
	cout << "Identifier : " << this->value << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
}

void Keyword::print()
{
	switch (this->value)
	{
	case KeywordType::IF:
	{
		cout << "Keyword : IF "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::ELSE:
	{
		cout << "Keyword : ELSE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::AND:
	{
		cout << "Keyword : AND "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::OR:
	{
		cout << "Keyword : OR "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::LET:
	{
		cout << "Keyword : LET "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::CONST:
	{
		cout << "Keyword : CONST "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::LOOP:
	{
		cout << "Keyword : LOOP "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::FOR:
	{
		cout << "Keyword : FOR "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::BREAK:
	{
		cout << "Keyword : BREAK "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::CONTINUE:
	{
		cout << "Keyword : CONTINUE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::FN:
	{
		cout << "Keyword : FN "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::RETURN:
	{
		cout << "Keyword : RETURN "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::NUM:
	{
		cout << "Keyword : NUM "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::STR:
	{
		cout << "Keyword : STR "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::BOOL:
	{
		cout << "Keyword : BOOL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::ENUM:
	{
		cout << "Keyword : ENUM "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::TYPE:
	{
		cout << "Keyword : TYPE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::OPT:
	{
		cout << "Keyword : OPT "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::TRUE:
	{
		cout << "Keyword : TRUE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::FALSE:
	{
		cout << "Keyword : FALSE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::IMPORT:
	{
		cout << "Keyword : IMPORT "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::FROM:
	{
		cout << "Keyword : FROM "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::PUB:
	{
		cout << "Keyword : PUB "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case KeywordType::PURE:
	{
		cout << "Keyword : PURE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	default:
		break;
	}
}

void Punctuation::print()
{
	switch (this->value)
	{
	case PunctuationType::OPEN_PARENTHESIS:
	{
		cout << "Punctuation : OPEN_PARENTHESIS "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::CLOSE_PARENTHESIS:
	{
		cout << "Punctuation : CLOSE_PARENTHESIS "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::OPEN_CURLY_BRACE:
	{
		cout << "Punctuation : OPEN_CURLY_BRACE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::CLOSE_CURLY_BRACE:
	{
		cout << "Punctuation : CLOSE_CURLY_BRACE "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::OPEN_BRACKET:
	{
		cout << "Punctuation : OPEN_BRACKET "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::CLOSE_BRACKET:
	{
		cout << "Punctuation : CLOSE_BRACKET "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::BACKWARD_SLASH:
	{
		cout << "Punctuation : BACKWARD_SLASH "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::SEMICOLON:
	{
		cout << "Punctuation : SEMICOLON "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::COMMA:
	{
		cout << "Punctuation : COMMA "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::COLON:
	{
		cout << "Punctuation : COLON "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::BAR:
	{
		cout << "Punctuation : BAR "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::ARROW:
	{
		cout << "Punctuation : ARROW "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::PLUS:
	{
		cout << "Punctuation : PLUS "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::PLUS_EQUAL:
	{
		cout << "Punctuation : PLUS_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::MINUS:
	{
		cout << "Punctuation : MINUS "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::MINUS_EQUAL:
	{
		cout << "Punctuation : MINUS_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::STAR:
	{
		cout << "Punctuation : STAR "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::STAR_EQUAL:
	{
		cout << "Punctuation : STAR_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::DIVISION:
	{
		cout << "Punctuation : DIVISION "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::DIVISION_EQUAL:
	{
		cout << "Punctuation : DIVISION_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::REMINDER:
	{
		cout << "Punctuation : REMINDER "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::REMINDER_EQUAL:
	{
		cout << "Punctuation : REMINDER_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::POWER:
	{
		cout << "Punctuation : POWER "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::POWER_EQUAL:
	{
		cout << "Punctuation : POWER_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::EQUAL:
	{
		cout << "Punctuation : EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::EQUAL_EQUAL:
	{
		cout << "Punctuation : EQUAL_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::BANG:
	{
		cout << "Punctuation : BANG "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::BANG_EQUAL:
	{
		cout << "Punctuation : BANG_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::LESSER_THAN:
	{
		cout << "Punctuation : LESSER_THAN "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::LESSER_THAN_EQUAL:
	{
		cout << "Punctuation : LESSER_THAN_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::GREATER_THAN:
	{
		cout << "Punctuation : GREATER_THAN "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	case PunctuationType::GREATER_THAN_EQUAL:
	{
		cout << "Punctuation : GREATER_THAN_EQUAL "
			 << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
		break;
	}
	default:
		break;
	}
}

void Unknown::print()
{
	cout << this->value << " - [Ln : " << this->line_num << " Col : " << this->column_num << "]" << endl;
}