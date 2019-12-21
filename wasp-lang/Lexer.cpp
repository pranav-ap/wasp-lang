#pragma once
#include <map>
#include <string>
#include "Lexer.h"

using namespace std;

map<string, KeywordType> keyword_map = {
	{"if", KeywordType::IF},
	{ "else", KeywordType::ELSE },

	{ "and", KeywordType::AND },
	{ "or", KeywordType::OR },

	{ "let", KeywordType::LET },
	{ "const", KeywordType::CONST },

	{ "loop", KeywordType::LOOP },
	{ "for", KeywordType::FOR },
	{ "break", KeywordType::BREAK },
	{ "continue", KeywordType::CONTINUE },

	{ "fn", KeywordType::FN },
	{ "return", KeywordType::RETURN },

	{ "num", KeywordType::NUM },
	{ "str", KeywordType::STR },
	{ "bool", KeywordType::BOOL },
	{ "enum", KeywordType::ENUM },
	{ "type", KeywordType::TYPE },
	{ "opt", KeywordType::OPT },

	{ "true", KeywordType::TRUE },
	{ "false", KeywordType::FALSE },

	{ "import", KeywordType::IMPORT },
	{ "from", KeywordType::FROM },

	{ "pub", KeywordType::PUB },
	{ "pure", KeywordType::PURE }
};

vector<unique_ptr<TokenType>> Lexer::execute()
{
	vector<unique_ptr<TokenType>> tokens;

	while (true)
	{
		char ch = this->get_current_char();

		if (ch == NULL)
		{
			break;
		}

		// skip whitespaces
		switch (ch)
		{
		case '\t':
		case ' ':
		{
			this->advance();
			this->pos.increment_column_number();
			continue;
		}

		case '\n':
		{
			this->advance();
			this->pos.increment_line_number();
			this->pos.reset_column_number();
			continue;
		}
		}

		if (std::isdigit(static_cast<unsigned char>(ch)))
		{
			auto token = this->consume_number_literal(ch);

			if (token != nullptr) {
				tokens.push_back(move(token));
			}
		}
		else if (std::isalpha(ch) || ch == '_')
		{
			auto token = this->consume_identifier(ch);

			if (keyword_map.count(token->value) > 0)
			{
				KeywordType keyword_type = keyword_map[token->value];
				token.reset();
				tokens.push_back(make_unique<Keyword>(keyword_type, this->pos.line_num, this->pos.column_num));
			} 
			else if (token != nullptr) 
			{
				tokens.push_back(move(token));
			}
		}
		else
		{
			switch (ch)
			{
			case '"':
			{
				auto token = this->consume_string_literal();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '\\':
			case ';':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case ',':
			case '.':
			case ':':
			case '|':
			{
				auto token = this->consume_single_char_punctuation(ch);

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '+':
			{
				auto token = this->handle_plus();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '-':
			{
				auto token = this->handle_minus();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '*':
			{
				auto token = this->handle_star();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '/':
			{
				auto token = this->handle_division();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '%':
			{
				auto token = this->handle_reminder();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '^':
			{
				auto token = this->handle_power();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '=':
			{
				auto token = this->handle_equal();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '!':
			{
				auto token = this->handle_bang();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '<':
			{
				auto token = this->handle_lesser_than();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			case '>':
			{
				auto token = this->handle_greater_than();

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}

			default:
			{
				auto token = this->consume_unknown_token(ch);

				if (token != nullptr) {
					tokens.push_back(move(token));
				}

				break;
			}
			}
		}
	}

	return tokens;
}

// Consumers

unique_ptr<NumberLiteral> Lexer::consume_number_literal(char ch)
{
	string number_literal;
	number_literal.push_back(ch);

	bool reached_decimal_point = false;

	while (ch = this->get_right_char())
	{
		if (isdigit(static_cast<unsigned char>(ch)) || (ch != '.' && !reached_decimal_point))
		{
			number_literal.push_back(ch);
			this->pos.increment_column_number();
			this->advance();
			continue;
		}

		break;
	}

	double number = stod(number_literal);

	return make_unique<NumberLiteral>(number, this->pos.line_num, this->pos.column_num);
}

unique_ptr<StringLiteral> Lexer::consume_string_literal()
{
	string string_literal;

	while (char ch = this->get_right_char())
	{
		if (ch != '"' && ch != '\n')
		{
			string_literal.push_back(ch);
			this->pos.increment_column_number();
			this->advance();
			break;
		}

		break;
	}

	return make_unique<StringLiteral>(string_literal, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Identifier> Lexer::consume_identifier(char ch)
{
	string identifier;
	identifier.push_back(ch);

	this->advance();

	while (ch = this->get_current_char())
	{
		if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '_' || std::isalpha(ch))
		{
			identifier.push_back(ch);
			this->pos.increment_column_number();
			this->advance();
			continue;
		}

		break;
	}

	bool is_function = this->get_right_char() == '(';

	return make_unique<Identifier>(identifier, is_function, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_plus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::PLUS_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::PLUS, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_minus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::MINUS_EQUAL, this->pos.line_num, this->pos.column_num);
	}
	else if (this->peek_and_move('>'))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::ARROW, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::MINUS, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_star()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::STAR_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::STAR, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_division()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::DIVISION_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::DIVISION, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_reminder()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::REMINDER_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::REMINDER, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_power()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::POWER_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::POWER, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_bang()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::BANG_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::BANG, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_equal()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::EQUAL_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::EQUAL, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_greater_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::GREATER_THAN_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::GREATER_THAN, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::handle_lesser_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(PunctuationType::LESSER_THAN_EQUAL, this->pos.line_num, this->pos.column_num);
	}

	this->advance();
	return make_unique<Punctuation>(PunctuationType::LESSER_THAN, this->pos.line_num, this->pos.column_num);
}

unique_ptr<Punctuation> Lexer::consume_single_char_punctuation(char ch)
{
	this->pos.increment_column_number();
	this->advance();

	switch (ch)
	{
	case '\\':
	{
		return make_unique<Punctuation>(PunctuationType::BACKWARD_SLASH, this->pos.line_num, this->pos.column_num);
	}
	case ';':
	{
		return make_unique<Punctuation>(PunctuationType::SEMICOLON, this->pos.line_num, this->pos.column_num);
	}
	case '(':
	{
		return make_unique<Punctuation>(PunctuationType::OPEN_PARENTHESIS, this->pos.line_num, this->pos.column_num);
	}
	case ')':
	{
		return make_unique<Punctuation>(PunctuationType::CLOSE_PARENTHESIS, this->pos.line_num, this->pos.column_num);
	}
	case '{':
	{
		return make_unique<Punctuation>(PunctuationType::OPEN_CURLY_BRACE, this->pos.line_num, this->pos.column_num);
	}
	case '}':
	{
		return make_unique<Punctuation>(PunctuationType::CLOSE_CURLY_BRACE, this->pos.line_num, this->pos.column_num);
	}
	case '[':
	{
		return make_unique<Punctuation>(PunctuationType::OPEN_BRACKET, this->pos.line_num, this->pos.column_num);
	}
	case ']':
	{
		return make_unique<Punctuation>(PunctuationType::CLOSE_BRACKET, this->pos.line_num, this->pos.column_num);
	}
	case ',':
	{
		return make_unique<Punctuation>(PunctuationType::COMMA, this->pos.line_num, this->pos.column_num);
	}
	case '.':
	{
		return make_unique<Punctuation>(PunctuationType::DOT, this->pos.line_num, this->pos.column_num);
	}
	case ':':
	{
		return make_unique<Punctuation>(PunctuationType::COLON, this->pos.line_num, this->pos.column_num);
	}
	case '|':
	{
		return make_unique<Punctuation>(PunctuationType::BAR, this->pos.line_num, this->pos.column_num);
	}
	}
}

unique_ptr<Unknown> Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

	while (char ch = this->get_right_char())
	{
		if (ch != ' ' && ch != '\n')
		{
			unknown_token.push_back(ch);
			this->pos.increment_column_number();
			this->advance();
			continue;
		}

		break;
	}

	return make_unique<Unknown>(unknown_token, this->pos.line_num, this->pos.column_num);
}

// UTILS

bool Lexer::peek_and_move(char ch)
{
	if (ch == this->get_right_char())
	{
		this->advance();
		return true;
	}

	return false;
}

void Lexer::advance()
{
	this->index++;
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
	return this->get_char_at(this->index);
}

char Lexer::get_right_char()
{
	return this->get_char_at(this->index + 1);
}

