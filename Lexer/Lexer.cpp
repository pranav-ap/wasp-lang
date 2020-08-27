#pragma once
#include "pch.h"
#include "Lexer.h"
#include "Assertion.h"
#include "utils.h"

#include <map>
#include <optional>
#include <memory>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>

#define BOM 65279
#define LEFT_ANGLE_BRACKET 10216
#define RIGHT_ANGLE_BRACKET 10217
#define LEFT_FLOOR_BRACKET 8970
#define RIGHT_FLOOR_BRACKET 8971

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
#define OPT_CHECK(x) ASSERT(x.has_value(), "Oh shit! Option is none")
#define MAKE_TOKEN(type, token, line_num, col_num) std::make_shared<Token>(type, token, line_num, col_num)
#define LINE_NUM position.get_line_num()
#define COL_NUM position.get_column_num()

#define CASE_BODY(call) \
	{                   \
		token = call;   \
		break;          \
	}

using std::make_optional;
using std::make_shared;
using std::map;
using std::move;
using std::nullopt;
using std::optional;
using std::string;
using std::wstring;
using std::vector;

void Lexer::init(std::wstring raw_source)
{
	this->raw_source = raw_source;
	tokens.clear();

	position = TokenPosition();
	pointer = Pointer();
}

vector<Token_ptr> Lexer::execute(std::wstring raw_source)
{
	init(raw_source);
	bool found_statement = false;

	while (true)
	{
		wchar_t current_char = get_current_char();

		if (current_char == NULL)
			break;

		if (current_char == BOM)
		{
			next();
			continue;
		}

		Token_ptr token;

		if (!found_statement && current_char == ' ')
		{
			next();
			token = consume_space();
		}
		else if (found_statement && current_char == ' ')
		{
			next();
			continue;
		}
		else if (current_char == '\n')
		{
			found_statement = false;
			next();
			token = consume_eol();
		}
		else if (iswdigit(current_char))
		{
			found_statement = true;
			next();
			token = consume_number_literal(current_char);
		}
		else if (iswalpha(current_char) || current_char == '_')
		{
			found_statement = true;
			next();
			token = consume_identifier(current_char);
		}
		else
		{
			found_statement = true;
			next();

			switch (current_char)
			{
			case '\\':

			case ',':
			case '|':
			case '.':

			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case LEFT_ANGLE_BRACKET:
			case RIGHT_ANGLE_BRACKET:
			case LEFT_FLOOR_BRACKET:
			case RIGHT_FLOOR_BRACKET:
				CASE_BODY(consume_single_char_punctuation(current_char));
			case '"':
				CASE_BODY(consume_string_literal());
			case '+':
				CASE_BODY(consume_plus());
			case '-':
				CASE_BODY(consume_minus());
			case '*':
				CASE_BODY(consume_star());
			case '/':
				CASE_BODY(consume_division());
			case '%':
				CASE_BODY(consume_reminder());
			case '^':
				CASE_BODY(consume_power());
			case '=':
				CASE_BODY(consume_equal());
			case '!':
				CASE_BODY(consume_bang());
			case '<':
				CASE_BODY(consume_lesser_than());
			case '>':
				CASE_BODY(consume_greater_than());
			case ':':
				CASE_BODY(consume_colon());
			default:
				CASE_BODY(consume_unknown_token(current_char));
			}
		}

		NULL_CHECK(token);
		tokens.push_back(move(token));
	}

	return tokens;
}

// Consumers

Token_ptr Lexer::consume_number_literal(wchar_t ch)
{
	std::wstring number_literal;
	number_literal.push_back(ch);

	bool reached_decimal_point = false;

	while (ch = get_current_char())
	{
		if (iswdigit(ch))
		{
			number_literal.push_back(ch);
			next();
			continue;
		}

		if (ch == '.')
		{
			if (reached_decimal_point)
				break;

			reached_decimal_point = true;

			number_literal.push_back(ch);
			next();
			continue;
		}

		break;
	}

	if (number_literal.back() == '.')
	{
		number_literal.pop_back();
		previous();
	}

	return MAKE_TOKEN(WTokenType::NUMBER_LITERAL, number_literal, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_string_literal()
{
	std::wstring string_literal;

	while (wchar_t ch = get_current_char())
	{
		if (ch == '"')
		{
			break;
		}

		string_literal.push_back(ch);
		next();
	}

	next();
	return MAKE_TOKEN(WTokenType::STRING_LITERAL, string_literal, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_identifier(wchar_t ch)
{
	std::wstring identifier;
	identifier.push_back(ch);

	while (ch = get_current_char())
	{
		if (iswdigit(ch) || ch == '_' || iswalpha(ch))
		{
			identifier.push_back(ch);
			next();
			continue;
		}

		break;
	}

	if (keyword_map.count(identifier) > 0)
	{
		WTokenType keyword_type = keyword_map.at(identifier);
		return MAKE_TOKEN(keyword_type, identifier, LINE_NUM, COL_NUM);
	}

	if (get_current_char() == '(')
	{
		return MAKE_TOKEN(WTokenType::CALLABLE_IDENTIFIER, identifier, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::IDENTIFIER, identifier, LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_plus()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::PLUS_EQUAL, L"+=", LINE_NUM, COL_NUM);

	if (is_unary())
		return MAKE_TOKEN(WTokenType::UNARY_PLUS, L"+", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::PLUS, L"+", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_minus()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::MINUS_EQUAL, L"-=", LINE_NUM, COL_NUM);
	else if (expect_current_char('>'))
		return MAKE_TOKEN(WTokenType::ARROW, L"->", LINE_NUM, COL_NUM);

	if (is_unary())
		return MAKE_TOKEN(WTokenType::UNARY_MINUS, L"-", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::MINUS, L"-", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_star()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::STAR_EQUAL, L"*=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::STAR, L"*", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_division()
{
	if (expect_current_char('='))
	{
		return MAKE_TOKEN(WTokenType::DIVISION_EQUAL, L"/=", LINE_NUM, COL_NUM);
	}
	else if (expect_current_char('/'))
	{
		std::wstring comment;

		while (wchar_t ch = get_current_char())
		{
			if (ch != '\n')
			{
				comment.push_back(ch);
				next();
				continue;
			}

			break;
		}

		return MAKE_TOKEN(WTokenType::COMMENT, comment, LINE_NUM, COL_NUM);
	}

	return MAKE_TOKEN(WTokenType::DIVISION, L"/", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_reminder()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::REMINDER_EQUAL, L"%=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::REMINDER, L"%", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_power()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::POWER_EQUAL, L"^=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::POWER, L"^", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_bang()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::BANG_EQUAL, L"!=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::BANG, L"!", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_equal()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::EQUAL_EQUAL, L"==", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::EQUAL, L"=", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_greater_than()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::GREATER_THAN_EQUAL, L">=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::GREATER_THAN, L">", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_lesser_than()
{
	if (expect_current_char('='))
		return MAKE_TOKEN(WTokenType::LESSER_THAN_EQUAL, L"<=", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::LESSER_THAN, L"<", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_colon()
{
	if (expect_current_char(':'))
		return MAKE_TOKEN(WTokenType::COLON_COLON, L"::", LINE_NUM, COL_NUM);

	return MAKE_TOKEN(WTokenType::COLON, L":", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_single_char_punctuation(wchar_t ch)
{
	switch (ch)
	{
	case '\\':
		return MAKE_TOKEN(WTokenType::BACKWARD_SLASH, L"\\", LINE_NUM, COL_NUM);

	case ',':
		return MAKE_TOKEN(WTokenType::COMMA, L",", LINE_NUM, COL_NUM);
	case '|':
		return MAKE_TOKEN(WTokenType::VERTICAL_BAR, L"|", LINE_NUM, COL_NUM);
	case '.':
		return MAKE_TOKEN(WTokenType::DOT, L".", LINE_NUM, COL_NUM);

	case '(':
		return MAKE_TOKEN(WTokenType::OPEN_PARENTHESIS, L"(", LINE_NUM, COL_NUM);
	case ')':
		return MAKE_TOKEN(WTokenType::CLOSE_PARENTHESIS, L")", LINE_NUM, COL_NUM);
	case '{':
		return MAKE_TOKEN(WTokenType::OPEN_CURLY_BRACE, L"{", LINE_NUM, COL_NUM);
	case '}':
		return MAKE_TOKEN(WTokenType::CLOSE_CURLY_BRACE, L"}", LINE_NUM, COL_NUM);
	case '[':
		return MAKE_TOKEN(WTokenType::OPEN_SQUARE_BRACKET, L"[", LINE_NUM, COL_NUM);
	case ']':
		return MAKE_TOKEN(WTokenType::CLOSE_SQUARE_BRACKET, L"]", LINE_NUM, COL_NUM);
	case LEFT_ANGLE_BRACKET:
		return MAKE_TOKEN(WTokenType::OPEN_ANGLE_BRACKET, L"⟨", LINE_NUM, COL_NUM);
	case RIGHT_ANGLE_BRACKET:
		return MAKE_TOKEN(WTokenType::CLOSE_ANGLE_BRACKET, L"⟩", LINE_NUM, COL_NUM);
	case LEFT_FLOOR_BRACKET:
		return MAKE_TOKEN(WTokenType::OPEN_FLOOR_BRACKET, L"⌊", LINE_NUM, COL_NUM);
	case RIGHT_FLOOR_BRACKET:
		return MAKE_TOKEN(WTokenType::CLOSE_FLOOR_BRACKET, L"⌋", LINE_NUM, COL_NUM);

	default:
		return MAKE_TOKEN(WTokenType::UNKNOWN, L"", LINE_NUM, COL_NUM);
	}
}

Token_ptr Lexer::consume_eol()
{
	int line_num = LINE_NUM;
	int column_num = COL_NUM + 1;

	position.increment_line_number();
	position.reset_column_number();

	return MAKE_TOKEN(WTokenType::EOL, L"\n", line_num, column_num);
}

Token_ptr Lexer::consume_space()
{
	return MAKE_TOKEN(WTokenType::SPACE, L"SPACE", LINE_NUM, COL_NUM);
}

Token_ptr Lexer::consume_unknown_token(wchar_t ch)
{
	std::wstring unknown_token;
	unknown_token.push_back(ch);

	for (ch = get_current_char(); ch != ' ' && ch != '\n';)
	{
		unknown_token.push_back(ch);
		next();
	}

	return MAKE_TOKEN(WTokenType::UNKNOWN, unknown_token, LINE_NUM, COL_NUM);
}

// UTILS

bool Lexer::expect_current_char(wchar_t ch)
{
	if (ch == get_current_char())
	{
		pointer.advance();
		return true;
	}

	return false;
}

wchar_t Lexer::get_char_at(int index) const
{
	if ((size_t)index >= raw_source.size() || index < 0)
		return NULL;

	wchar_t ch = raw_source.at((size_t)index);
	return ch;
}

wchar_t Lexer::get_current_char() const
{
	int index = pointer.get_index();
	return get_char_at(index);
}

wchar_t Lexer::get_right_char() const
{
	int index = pointer.get_index();
	return get_char_at(index + 1);
}

optional<Token_ptr> Lexer::get_previous_significant_token()
{
	for (auto t = tokens.rbegin(); t != tokens.rend(); t++)
	{
		if (auto token = *t; token->type != WTokenType::EOL)
		{
			return make_optional(token);
		}
	}

	return nullopt;
}

bool Lexer::is_unary()
{
	auto previous_token = get_previous_significant_token();

	if (!previous_token.has_value())
		return true;

	switch (previous_token.value()->type)
	{
	case WTokenType::NUMBER_LITERAL:
	case WTokenType::IDENTIFIER:
	case WTokenType::CALLABLE_IDENTIFIER:
	{
		return false;
	}

	default:
	{
		return true;
	}
	}
}

void Lexer::next()
{
	pointer.advance();
	position.increment_column_number();
}

void Lexer::previous()
{
	pointer.retreat();
	position.decrement_column_number();
}