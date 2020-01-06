#pragma once
#include "Lexer.h"

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

vector<Token_ptr> Lexer::execute()
{
	vector<Token_ptr> tokens;

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
			tokens.push_back(move(token));
		}
	}

	return tokens;
}

// Consumers

Token_ptr Lexer::consume_number_literal(char ch)
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

Token_ptr Lexer::consume_string_literal()
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
		TokenType keyword_type = keyword_map[identifier];
		return MAKE_TOKEN(keyword_type, identifier, LINE_NUM, COL_NUM);
	}

	if (this->followed_by('(')) {
		return MAKE_TOKEN(TokenType::FunctionIdentifier, identifier, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(TokenType::Identifier, identifier, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_plus()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::PLUS_EQUAL, "+=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::PLUS, "+", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_minus()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::MINUS_EQUAL, "-=", LINE_NUM, COL_NUM);
	else if (this->expect_current_char('>'))
		return MAKE_TOKEN(TokenType::ARROW, "->", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::MINUS, "-", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_star()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::STAR_EQUAL, "*=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::STAR, "*", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_division()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::DIVISION_EQUAL, "/=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::DIVISION, "/", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_reminder()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::REMINDER_EQUAL, "%=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::REMINDER, "%", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_power()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::POWER_EQUAL, "^=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::POWER, "^", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_bang()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::BANG_EQUAL, "!=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::BANG, "!", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_equal()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::EQUAL_EQUAL, "==", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::EQUAL, "=", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_greater_than()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::GREATER_THAN_EQUAL, ">=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::GREATER_THAN, ">", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_lesser_than()
{
	if (this->expect_current_char('='))
		return MAKE_TOKEN(TokenType::LESSER_THAN_EQUAL, "<=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(TokenType::LESSER_THAN, "<", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::handle_dot()
{
	if (this->expect_current_char('.'))
	{
		if (this->expect_current_char('.'))
			return MAKE_TOKEN(TokenType::DOT_DOT_DOT, "...", LINE_NUM, COL_NUM);

		return MAKE_TOKEN(TokenType::DOT_DOT, "..", LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(TokenType::DOT, ".", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_single_char_punctuation(char ch)
{
	switch (ch)
	{
	case '\\':
		return MAKE_TOKEN(TokenType::BACKWARD_SLASH, "\\", LINE_NUM, COL_NUM);
	case '(':
		return MAKE_TOKEN(TokenType::OPEN_PARENTHESIS, "(", LINE_NUM, COL_NUM);
	case ')':
		return MAKE_TOKEN(TokenType::CLOSE_PARENTHESIS, ")", LINE_NUM, COL_NUM);
	case '{':
		return MAKE_TOKEN(TokenType::OPEN_CURLY_BRACE, "{", LINE_NUM, COL_NUM);
	case '}':
		return MAKE_TOKEN(TokenType::CLOSE_CURLY_BRACE, "}", LINE_NUM, COL_NUM);
	case '[':
		return MAKE_TOKEN(TokenType::OPEN_BRACKET, "[", LINE_NUM, COL_NUM);
	case ']':
		return MAKE_TOKEN(TokenType::CLOSE_BRACKET, "]", LINE_NUM, COL_NUM);
	case ',':
		return MAKE_TOKEN(TokenType::COMMA, ",", LINE_NUM, COL_NUM);
	case '.':
		return MAKE_TOKEN(TokenType::DOT, ".", LINE_NUM, COL_NUM);
	case ':':
		return MAKE_TOKEN(TokenType::COLON, ":", LINE_NUM, COL_NUM);
	case '|':
		return MAKE_TOKEN(TokenType::BAR, "|", LINE_NUM, COL_NUM);
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

	return MAKE_TOKEN(TokenType::EOL, "\\n", line_num, column_num);
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

	return MAKE_TOKEN(TokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}