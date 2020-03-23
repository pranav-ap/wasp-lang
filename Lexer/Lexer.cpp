#pragma once
#include "pch.h"
#include "Lexer.h"
#include <map>
#include <memory>
#include <string>

using std::string;
using std::make_shared;

std::map<std::string, WTokenType> keyword_map = {
	{ "if", WTokenType::IF },
	{ "else", WTokenType::ELSE },

	{ "and", WTokenType::AND },
	{ "or", WTokenType::OR },

	{ "let", WTokenType::LET },
	{ "const", WTokenType::CONST_KEYWORD },

	{ "loop", WTokenType::LOOP },
	{ "for", WTokenType::FOR },
	{ "break", WTokenType::BREAK },
	{ "continue", WTokenType::CONTINUE },
	{ "in", WTokenType::IN_KEYWORD },

	{ "fn", WTokenType::FN },
	{ "return", WTokenType::RETURN },

	{ "num", WTokenType::NUM },
	{ "str", WTokenType::STR },
	{ "bool", WTokenType::BOOL },
	{ "enum", WTokenType::ENUM },
	{ "type", WTokenType::TYPE },
	{ "opt", WTokenType::OPT },

	{ "none", WTokenType::NONE },

	{ "true", WTokenType::TRUE_KEYWORD },
	{ "false", WTokenType::FALSE_KEYWORD },

	{ "import", WTokenType::IMPORT },
	{ "from", WTokenType::FROM },

	{ "pub", WTokenType::PUB }
};

std::vector<Token_ptr> Lexer::execute()
{
	std::vector<Token_ptr> tokens;

	while (true)
	{
		char ch = this->get_current_char();

		if (ch == NULL)
			break;

		// skip whitespaces
		if (ch == ' ' || ch == '\t')
		{
			NEXT;
			continue;
		}

		Token_ptr token;

		if (std::isdigit(static_cast<unsigned char>(ch)))
		{
			NEXT;
			token = this->consume_number_literal(ch);
		}
		else if (std::isalpha(ch) || ch == '_')
		{
			NEXT;
			token = this->consume_identifier(ch);
		}
		else if (ch == '\n')
		{
			NEXT;
			token = this->consume_eol();
		}
		else
		{
			NEXT;

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
			tokens.push_back(std::move(token));
		}
	}

	return tokens;
}

// Consumers

Token_ptr Lexer::consume_number_literal(char ch)
{
	std::string number_literal;
	number_literal.push_back(ch);

	while (ch = this->get_current_char())
	{
		if (isdigit(static_cast<unsigned char>(ch)) || ch == '.')
		{
			number_literal.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	// TODO: What if there are multiple '.'

	return MAKE_TOKEN(WTokenType::NumberLiteral, number_literal, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_string_literal()
{
	std::string string_literal;

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
	return MAKE_TOKEN(WTokenType::StringLiteral, string_literal, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_identifier(char ch)
{
	string identifier;
	identifier.push_back(ch);

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
		WTokenType keyword_type = keyword_map[identifier];
		return MAKE_TOKEN(keyword_type, identifier, LINE_NUM, COL_NUM);
	}

	if (this->get_right_char() == '(') {
		return MAKE_TOKEN(WTokenType::FunctionIdentifier, identifier, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::Identifier, identifier, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_plus()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::PLUS_EQUAL, "+=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::PLUS, "+", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_minus()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::MINUS_EQUAL, "-=", LINE_NUM, COL_NUM);
	else if (this->expect_current_char('>'))
		return MAKE_TOKEN(WTokenType::ARROW, "->", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::MINUS, "-", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_star()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::STAR_EQUAL, "*=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::STAR, "*", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_division()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::DIVISION_EQUAL, "/=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::DIVISION, "/", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_reminder()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::REMINDER_EQUAL, "%=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::REMINDER, "%", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_power()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::POWER_EQUAL, "^=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::POWER, "^", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_bang()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::BANG_EQUAL, "!=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::BANG, "!", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_equal()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::EQUAL_EQUAL, "==", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::EQUAL, "=", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_greater_than()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::GREATER_THAN_EQUAL, ">=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::GREATER_THAN, ">", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_lesser_than()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(WTokenType::LESSER_THAN_EQUAL, "<=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::LESSER_THAN, "<", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_dot()
{
	if (this->expect_current_char('.'))
	{
		if (this->expect_current_char('.'))
			return MAKE_TOKEN(WTokenType::DOT_DOT_DOT, "...", LINE_NUM, COL_NUM);

		return MAKE_TOKEN(WTokenType::DOT_DOT, "..", LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::DOT, ".", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_single_char_punctuation(char ch)
{
	switch (ch)
	{
	case '\\':
		return MAKE_TOKEN(WTokenType::BACKWARD_SLASH, "\\", LINE_NUM, COL_NUM);
	case '(':
		return MAKE_TOKEN(WTokenType::OPEN_PARENTHESIS, "(", LINE_NUM, COL_NUM);
	case ')':
		return MAKE_TOKEN(WTokenType::CLOSE_PARENTHESIS, ")", LINE_NUM, COL_NUM);
	case '{':
		return MAKE_TOKEN(WTokenType::OPEN_CURLY_BRACE, "{", LINE_NUM, COL_NUM);
	case '}':
		return MAKE_TOKEN(WTokenType::CLOSE_CURLY_BRACE, "}", LINE_NUM, COL_NUM);
	case '[':
		return MAKE_TOKEN(WTokenType::OPEN_BRACKET, "[", LINE_NUM, COL_NUM);
	case ']':
		return MAKE_TOKEN(WTokenType::CLOSE_BRACKET, "]", LINE_NUM, COL_NUM);
	case ',':
		return MAKE_TOKEN(WTokenType::COMMA, ",", LINE_NUM, COL_NUM);
	case '.':
		return MAKE_TOKEN(WTokenType::DOT, ".", LINE_NUM, COL_NUM);
	case ':':
		return MAKE_TOKEN(WTokenType::COLON, ":", LINE_NUM, COL_NUM);
	case '|':
		return MAKE_TOKEN(WTokenType::BAR, "|", LINE_NUM, COL_NUM);
	default:
		return nullptr;
	}
}

Token_ptr Lexer::consume_eol()
{
	int line_num = LINE_NUM;
	int column_num = COL_NUM + 1;

	this->position.increment_line_number();
	this->position.reset_column_number();

	return MAKE_TOKEN(WTokenType::EOL, "\\n", line_num, column_num);
}

Token_ptr Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

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

	return MAKE_TOKEN(WTokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}