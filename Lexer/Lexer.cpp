#pragma once
#include "pch.h"
#include "Lexer.h"
#include "spdlog.h"

#include <map>
#include <memory>
#include <algorithm>
#include <string>
#include <vector>

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
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");

	bool found_statement = false;

	while (true)
	{
		char current_char = get_current_char();

		if (current_char == NULL)
			break;

		Token_ptr token;

		if (!found_statement && current_char == ' ')
		{
			NEXT;
			token = consume_space();
		}
		else if (found_statement && current_char == ' ')
		{
			NEXT;
			continue;
		}
		else if (current_char == '\n')
		{
			found_statement = false;
			NEXT;
			token = consume_eol();
		}
		else if (isdigit(static_cast<unsigned char>(current_char)))
		{
			found_statement = true;
			NEXT;
			token = consume_number_literal(current_char);
		}
		else if (isalpha(current_char) || current_char == '_')
		{
			found_statement = true;
			NEXT;
			token = consume_identifier(current_char);
		}
		else
		{
			found_statement = true;
			NEXT;

			switch (current_char)
			{
			case '\\':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case ',':
			case '|': CASE_BODY(consume_single_char_punctuation(current_char));
			case '(': CASE_BODY(consume_open_parenthesis());
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
			default: CASE_BODY(consume_unknown_token(current_char));
			}
		}

		if (token)
		{
			//spdlog::info("Ln {} Col {}\t: {}", token->line_num, token->column_num, token->value);
			tokens.push_back(move(token));
		}
		else
		{
			spdlog::error("A Token Consumer returns nullptr");
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
		return MAKE_TOKEN(WTokenType::COLON_COLON, "::", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::COLON, ":", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_open_parenthesis()
{
	if (expect_current_char('@'))
		return MAKE_TOKEN(WTokenType::OPEN_TUPLE_PARENTHESIS, "(@", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::OPEN_PARENTHESIS, "(", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_single_char_punctuation(char ch)
{
	switch (ch)
	{
	case '\\':
		return MAKE_TOKEN(WTokenType::BACKWARD_SLASH, "\\", LINE_NUM, COL_NUM);
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

Token_ptr Lexer::consume_space()
{
	return MAKE_TOKEN(WTokenType::SPACE, "SPACE", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

	for (ch = get_current_char(); ch != ' ' && ch != '\n';)
	{
		unknown_token.push_back(ch);
		NEXT;
	}

	return MAKE_TOKEN(WTokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}

// UTILS

bool Lexer::expect_current_char(char ch)
{
	if (ch == get_current_char())
	{
		pointer.advance();
		return true;
	}

	return false;
}

char Lexer::get_char_at(int index) const
{
	if ((size_t)index >= raw_source.size() || index < 0)
		return NULL;

	return raw_source[index];
}

char Lexer::get_current_char() const
{
	int index = pointer.get_index();
	return get_char_at(index);
}

char Lexer::get_right_char() const
{
	int index = pointer.get_index();
	return get_char_at(index + 1);
}

Token_ptr Lexer::get_previous_significant_token()
{
	Token_ptr previous_token = nullptr;

	for (auto t = tokens.rbegin(); t != tokens.rend(); t++)
	{
		if (auto token = *t; token->type != WTokenType::EOL)
		{
			previous_token = token;
			break;
		}
	}

	return previous_token;
}

bool Lexer::is_unary()
{
	Token_ptr previous_token = get_previous_significant_token();

	if (previous_token == nullptr)
		return true;

	switch (previous_token->type)
	{
	case WTokenType::NumberLiteral:
	case WTokenType::Identifier:
	case WTokenType::FunctionIdentifier:
	{
		return false;
	}

	default:
	{
		return true;
	}
	}
}