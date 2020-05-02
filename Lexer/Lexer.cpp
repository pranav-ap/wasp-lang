#pragma once
#include "pch.h"
#include "Lexer.h"
#include "spdlog.h"

#include <map>
#include <memory>
#include <string>

using std::string;
using std::to_string;
using std::map;
using std::vector;
using std::move;
using std::make_shared;
using std::isdigit;
using std::isalpha;

#define MAKE_TOKEN(type, token, line_num, col_num) std::make_shared<Token>(type, token, line_num, col_num)
#define CASE_BODY(call) { token = call; break; }
#define NEXT pointer.advance(); position.increment_column_number();
#define LINE_NUM position.get_line_num()
#define COL_NUM position.get_column_num()

vector<Token_ptr> Lexer::execute()
{
	while (true)
	{
		char ch = get_current_char();

		if (ch == NULL)
			break;

		if (ch == ' ' && tokens.back()->type != WTokenType::EOL)
		{
			NEXT;
			continue;
		}

		Token_ptr token;

		if (isdigit(static_cast<unsigned char>(ch)))
		{
			NEXT;
			token = consume_number_literal(ch);
		}
		else if (isalpha(ch) || ch == '_')
		{
			NEXT;
			token = consume_identifier(ch);
		}
		else if (ch == '\n')
		{
			NEXT;
			token = consume_eol();
		}
		else if (ch == ' ')
		{
			NEXT;
			token = consume_indent();
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
			case '|': CASE_BODY(consume_single_char_punctuation(ch));
			case '"': CASE_BODY(consume_string_literal());
			case '+': CASE_BODY(consume_plus());
			case '-': CASE_BODY(consume_minus());
			case '*': CASE_BODY(consume_star());
			case '/': CASE_BODY(consume_division());
			case '%': CASE_BODY(consume_reminder());
			case '^': CASE_BODY(consume_power());
			case '=': CASE_BODY(consume_equal());
			case '!': CASE_BODY(consume_bang());
			case '<': CASE_BODY(consume_lesser_than());
			case '>': CASE_BODY(consume_greater_than());
			case '.': CASE_BODY(consume_dot());
			case ':': CASE_BODY(consume_colon());
			default: CASE_BODY(consume_unknown_token(ch));
			}
		}

		if (token)
		{
			spdlog::info("Ln {} Col {} : {}", token->line_num, token->column_num, token->value);
			tokens.push_back(move(token));
		}
		else
		{
			spdlog::error("Token == nullptr");
			exit(1);
		}
	}

	return tokens;
}

// Consumers

Token_ptr Lexer::consume_number_literal(char ch)
{
	string number_literal;
	number_literal.push_back(ch);

	bool reached_decimal_point = false;

	while (ch = get_current_char())
	{
		if (isdigit(static_cast<unsigned char>(ch)))
		{
			number_literal.push_back(ch);
			NEXT;
			continue;
		}

		if (ch == '.')
		{
			if (reached_decimal_point)
			{
				spdlog::error("Multiple decimal points are detected");
				NEXT;
				return nullptr;
			}

			reached_decimal_point = true;

			number_literal.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	return MAKE_TOKEN(WTokenType::NumberLiteral, number_literal, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_string_literal()
{
	string string_literal;

	while (char ch = get_current_char())
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

	while (ch = get_current_char())
	{
		if (isdigit(static_cast<unsigned char>(ch)) || ch == '_' || isalpha(ch))
		{
			identifier.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	if (keyword_map.count(identifier) > 0)
	{
		WTokenType keyword_type = keyword_map.at(identifier);
		return MAKE_TOKEN(keyword_type, identifier, LINE_NUM, COL_NUM);
	}

	if (get_current_char() == '(') {
		return MAKE_TOKEN(WTokenType::FunctionIdentifier, identifier, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::Identifier, identifier, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_plus()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::PLUS_EQUAL, "+=", LINE_NUM, COL_NUM);

	if (is_unary())
		return MAKE_TOKEN(WTokenType::UNARY_PLUS, "+", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::PLUS, "+", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_minus()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::MINUS_EQUAL, "-=", LINE_NUM, COL_NUM);
	else if (expect_current_char('>'))
		return MAKE_TOKEN(WTokenType::ARROW, "->", LINE_NUM, COL_NUM);

	if (is_unary())
		return MAKE_TOKEN(WTokenType::UNARY_MINUS, "-", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::MINUS, "-", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_star()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::STAR_EQUAL, "*=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::STAR, "*", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_division()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::DIVISION_EQUAL, "/=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::DIVISION, "/", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_reminder()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::REMINDER_EQUAL, "%=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::REMINDER, "%", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_power()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::POWER_EQUAL, "^=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::POWER, "^", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_bang()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::BANG_EQUAL, "!=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::BANG, "!", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_equal()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::EQUAL_EQUAL, "==", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::EQUAL, "=", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_greater_than()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::GREATER_THAN_EQUAL, ">=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::GREATER_THAN, ">", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_lesser_than()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::LESSER_THAN_EQUAL, "<=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::LESSER_THAN, "<", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_dot()
{
	if (expect_current_char('.'))
	{
		if (expect_current_char('.'))
			return MAKE_TOKEN(WTokenType::DOT_DOT_DOT, "...", LINE_NUM, COL_NUM);

		return MAKE_TOKEN(WTokenType::DOT_DOT, "..", LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::DOT, ".", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_colon()
{
	if (expect_current_char(':'))
	{
		return MAKE_TOKEN(WTokenType::COLON_COLON, "::", LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::COLON, ":", LINE_NUM, COL_NUM);
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
		return MAKE_TOKEN(WTokenType::OPEN_SQUARE_BRACKET, "[", LINE_NUM, COL_NUM);
	case ']':
		return MAKE_TOKEN(WTokenType::CLOSE_SQUARE_BRACKET, "]", LINE_NUM, COL_NUM);
	case ',':
		return MAKE_TOKEN(WTokenType::COMMA, ",", LINE_NUM, COL_NUM);
	case '|':
		return MAKE_TOKEN(WTokenType::BAR, ",", LINE_NUM, COL_NUM);
	case '.':
		return MAKE_TOKEN(WTokenType::DOT, ".", LINE_NUM, COL_NUM);
	}
}

Token_ptr Lexer::consume_eol()
{
	int line_num = LINE_NUM;
	int column_num = COL_NUM + 1;

	position.increment_line_number();
	position.reset_column_number();

	return MAKE_TOKEN(WTokenType::EOL, "\\n", line_num, column_num);
}

Token_ptr Lexer::consume_indent()
{
	int num_of_spaces = 1;

	while (expect_current_char(' '))
	{
		num_of_spaces++;

		if (num_of_spaces == 4)
		{
			return MAKE_TOKEN(WTokenType::INDENT, "INDENT", LINE_NUM, COL_NUM);
		}
	}

	if (!expect_current_char('\n')) {
		spdlog::error("Indentation level is wrong");
		exit(1);
	}

	return MAKE_TOKEN(WTokenType::EOL, "\\n", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

	while (ch = get_current_char())
	{
		if (ch != ' ' && ch != '\n')
		{
			unknown_token.push_back(ch);
			NEXT;
			continue;
		}

		break;
	}

	string message = unknown_token + " I have NO idea what this token is! Fix it!";
	ERROR(message);

	return MAKE_TOKEN(WTokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}