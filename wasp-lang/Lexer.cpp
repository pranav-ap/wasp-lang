#pragma once
#include <map>
#include "Lexer.h"

#define MAKE_TOKEN(a, b, c, d) make_shared<Token>(a, b, c, d)
#define CASE_BODY(call) { token = call; break; }
#define NEXT this->pointer.advance(); this->position.increment_column_number();
#define LINE_NUM this->position.get_line_num()
#define COL_NUM this->position.get_column_num()

using std::map;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

map<string, TokenType> keyword_map = {
	{ "if", TokenType::IF },
	{ "else", TokenType::ELSE },

	{ "and", TokenType::AND },
	{ "or", TokenType::OR },

	{ "let", TokenType::LET },
	{ "const", TokenType::CONST },

	{ "loop", TokenType::LOOP },
	{ "for", TokenType::FOR },
	{ "break", TokenType::BREAK },
	{ "continue", TokenType::CONTINUE },
	{ "in", TokenType::IN },

	{ "fn", TokenType::FN },
	{ "return", TokenType::RETURN },

	{ "num", TokenType::NUM },
	{ "str", TokenType::STR },
	{ "bool", TokenType::BOOL },
	{ "enum", TokenType::ENUM },
	{ "type", TokenType::TYPE },
	{ "opt", TokenType::OPT },

	{ "none", TokenType::NONE },

	{ "true", TokenType::TRUE },
	{ "false", TokenType::FALSE },

	{ "import", TokenType::IMPORT },
	{ "from", TokenType::FROM },

	{ "pub", TokenType::PUB },
	{ "pure", TokenType::PURE },

	{ "pass", TokenType::PASS }
};

vector<shared_ptr<Token>> Lexer::execute()
{
	vector<shared_ptr<Token>> tokens;

	while (true)
	{
		char ch = this->get_current_char();

		if (ch == NULL)
		{
			break;
		}

		// skip whitespaces
		if (ch == ' ' || ch == '\t')
		{
			NEXT;
			continue;
		}

		shared_ptr<Token> token;

		if (std::isdigit(static_cast<unsigned char>(ch)))
		{
			token = this->consume_number_literal(ch);
		}
		else if (std::isalpha(ch) || ch == '_')
		{
			token = this->consume_identifier(ch);
		}
		else if (ch == '\n')
		{
			token = this->consume_eol();
		}
		else
		{
			switch (ch)
			{
			case '\\':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case ',':
			case ':':
			case '|': CASE_BODY(this->consume_single_char_punctuation(ch));
			case '"': CASE_BODY(this->consume_string_literal());
			case '+': CASE_BODY(this->handle_plus());
			case '-': CASE_BODY(this->handle_minus());
			case '*': CASE_BODY(this->handle_star());
			case '/': CASE_BODY(this->handle_division());
			case '%': CASE_BODY(this->handle_reminder());
			case '^': CASE_BODY(this->handle_power());
			case '=': CASE_BODY(this->handle_equal());
			case '!': CASE_BODY(this->handle_bang());
			case '<': CASE_BODY(this->handle_lesser_than());
			case '>': CASE_BODY(this->handle_greater_than());
			case '.': CASE_BODY(this->handle_dot());
			default: CASE_BODY(this->consume_unknown_token(ch));
			}
		}

		if (token != nullptr)
		{
			tokens.push_back(move(token));
		}
	}

	return tokens;
}

// Consumers

shared_ptr<Token> Lexer::consume_number_literal(char ch)
{
	string number_literal;
	number_literal.push_back(ch);

	while (ch = this->get_right_char())
	{
		if (isdigit(static_cast<unsigned char>(ch)) || ch == '.')
		{
			number_literal.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	NEXT;

	return MAKE_TOKEN(TokenType::NumberLiteral, number_literal, LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::consume_string_literal()
{
	string string_literal;

	NEXT;

	while (char ch = this->get_current_char())
	{
		if (ch == '"')
		{
			break;
		}

		string_literal.push_back(ch);
		NEXT;
	}

	NEXT;
	return MAKE_TOKEN(TokenType::StringLiteral, string_literal, LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::consume_identifier(char ch)
{
	string identifier;
	identifier.push_back(ch);

	NEXT;

	while (ch = this->get_current_char())
	{
		if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '_' || std::isalpha(ch))
		{
			identifier.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	if (keyword_map.count(identifier) > 0)
	{
		TokenType keyword_type = keyword_map[identifier];
		return MAKE_TOKEN(keyword_type, identifier, LINE_NUM, COL_NUM);
	}

	if (this->get_current_char() == '(') {
		return MAKE_TOKEN(TokenType::FunctionIdentifier, identifier, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(TokenType::Identifier, identifier, LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_plus()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::PLUS_EQUAL, "+=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::PLUS, "+", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_minus()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::MINUS_EQUAL, "-=", LINE_NUM, COL_NUM);
	}
	else if (this->peek_and_move('>'))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::ARROW, "->", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::MINUS, "-", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_star()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::STAR_EQUAL, "*=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::STAR, "*", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_division()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::DIVISION_EQUAL, "/=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::DIVISION, "/", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_reminder()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::REMINDER_EQUAL, "%=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::REMINDER, "%", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_power()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::POWER_EQUAL, "^=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::POWER, "^", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_bang()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::BANG_EQUAL, "!=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::BANG, "!", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_equal()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::EQUAL_EQUAL, "==", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::EQUAL, "=", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_greater_than()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::GREATER_THAN_EQUAL, ">=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::GREATER_THAN, ">", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_lesser_than()
{
	if (this->peek_and_move('='))
	{
		NEXT;
		return MAKE_TOKEN(TokenType::LESSER_THAN_EQUAL, "<=", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::LESSER_THAN, "<", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::handle_dot()
{
	if (this->peek_and_move('.'))
	{
		if (this->peek_and_move('.'))
		{
			NEXT;
			return MAKE_TOKEN(TokenType::DOT_DOT_DOT, "...", LINE_NUM, COL_NUM);
		}

		NEXT;
		return MAKE_TOKEN(TokenType::DOT_DOT, "..", LINE_NUM, COL_NUM);
	}

	NEXT;
	return MAKE_TOKEN(TokenType::DOT, ".", LINE_NUM, COL_NUM);
}

shared_ptr<Token> Lexer::consume_single_char_punctuation(char ch)
{
	NEXT;

	switch (ch)
	{
	case '\\':
	{
		return MAKE_TOKEN(TokenType::BACKWARD_SLASH, "\\", LINE_NUM, COL_NUM);
	}
	case '(':
	{
		return MAKE_TOKEN(TokenType::OPEN_PARENTHESIS, "(", LINE_NUM, COL_NUM);
	}
	case ')':
	{
		return MAKE_TOKEN(TokenType::CLOSE_PARENTHESIS, ")", LINE_NUM, COL_NUM);
	}
	case '{':
	{
		return MAKE_TOKEN(TokenType::OPEN_CURLY_BRACE, "{", LINE_NUM, COL_NUM);
	}
	case '}':
	{
		return MAKE_TOKEN(TokenType::CLOSE_CURLY_BRACE, "}", LINE_NUM, COL_NUM);
	}
	case '[':
	{
		return MAKE_TOKEN(TokenType::OPEN_BRACKET, "[", LINE_NUM, COL_NUM);
	}
	case ']':
	{
		return MAKE_TOKEN(TokenType::CLOSE_BRACKET, "]", LINE_NUM, COL_NUM);
	}
	case ',':
	{
		return MAKE_TOKEN(TokenType::COMMA, ",", LINE_NUM, COL_NUM);
	}
	case '.':
	{
		return MAKE_TOKEN(TokenType::DOT, ".", LINE_NUM, COL_NUM);
	}
	case ':':
	{
		return MAKE_TOKEN(TokenType::COLON, ":", LINE_NUM, COL_NUM);
	}
	case '|':
	{
		return MAKE_TOKEN(TokenType::BAR, "|", LINE_NUM, COL_NUM);
	}
	default:
		return nullptr;
	}
}

shared_ptr<Token> Lexer::consume_eol()
{
	int line_num = LINE_NUM;
	int column_num = COL_NUM + 1;

	this->pointer.advance();
	this->position.increment_line_number();
	this->position.reset_column_number();

	return MAKE_TOKEN(TokenType::EOL, "\\n", line_num, column_num);
}

shared_ptr<Token> Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

	NEXT;
	while (ch = this->get_current_char())
	{
		if (ch != ' ' && ch != '\n')
		{
			unknown_token.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	return MAKE_TOKEN(TokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}

// UTILS

bool Lexer::peek_and_move(char ch)
{
	if (ch == this->get_right_char())
	{
		this->pointer.advance();
		return true;
	}

	return false;
}

char Lexer::get_char_at(int index)
{
	if ((size_t)index >= this->raw_source.size())
	{
		return NULL;
	}

	return this->raw_source[index];
}

char Lexer::get_current_char()
{
	return this->get_char_at(this->pointer.get_index());
}

char Lexer::get_right_char()
{
	return this->get_char_at(this->pointer.get_index() + 1);
}