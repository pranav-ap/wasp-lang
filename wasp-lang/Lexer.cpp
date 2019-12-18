#include "Lexer.h"
#include <iostream>

using namespace std;

vector<unique_ptr<Token>> Lexer::execute()
{
	vector<unique_ptr<Token>> tokens;

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

		unique_ptr<Token> token;

		if (std::isdigit(static_cast<unsigned char>(ch)))
		{
			token = this->consume_number_literal(ch);
		}
		else if (std::isalpha(ch) || ch == '_')
		{
			token = this->consume_identifier(ch);
		}
		else
		{
			switch (ch)
			{
			case '"':
			{
				token = this->consume_string_literal();
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
				token = this->consume_single_char_punctuation(ch);
				break;
			}

			case '+':
			{
				token = this->handle_plus();
				break;
			}

			case '-':
			{
				token = this->handle_minus();
				break;
			}

			case '*':
			{
				token = this->handle_star();
				break;
			}

			case '/':
			{
				token = this->handle_division();
				break;
			}

			case '%':
			{
				token = this->handle_reminder();
				break;
			}

			case '^':
			{
				token = this->handle_power();
				break;
			}

			case '=':
			{
				token = this->handle_equal();
				break;
			}

			case '!':
			{
				token = this->handle_bang();
				break;
			}

			case '<':
			{
				token = this->handle_lesser_than();
				break;
			}

			case '>':
			{
				token = this->handle_greater_than();
				break;
			}

			default:
			{
				token = this->consume_unknown_token(ch);
				break;
			}
			}
		}

		if (token != NULL)
		{
			tokens.push_back(token);
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
		if (std::isdigit(static_cast<unsigned char>(ch)) || (ch != '.' && !reached_decimal_point))
		{
			number_literal.push_back(ch);
			this->pos.increment_column_number();
			this->advance();
			continue;
		}

		break;
	}

	double number = std::stod(number_literal);

	return make_unique<NumberLiteral>(new NumberLiteral(number, this->pos.to_string()));
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

	return make_unique<StringLiteral>(new StringLiteral(string_literal, this->pos.to_string()));
}

unique_ptr<Token> Lexer::consume_identifier(char ch)
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

	if (keyword_map.count(identifier) > 0)
	{
		KeywordType keyword_type = keyword_map[identifier];
		return make_unique<Keyword>(new Keyword(keyword_type, this->pos.to_string()));
	}

	if (this->get_right_char() == '(')
	{
		return make_unique<FunctionIdentifier>(new FunctionIdentifier(identifier, this->pos.to_string()));
	}

	return make_unique<Identifier>(new Identifier(identifier, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_plus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(PLUS_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(PLUS, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_minus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(MINUS_EQUAL, this->pos.to_string()));
	}
	else if (this->peek_and_move('>'))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(ARROW, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(MINUS, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_star()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(STAR_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(STAR, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_division()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(DIVISION_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(DIVISION, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_reminder()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(REMINDER_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(REMINDER, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_power()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(POWER_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(POWER, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_bang()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(BANG_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(BANG, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_equal()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(EQUAL_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(EQUAL, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_greater_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(GREATER_THAN_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(GREATER_THAN, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::handle_lesser_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<Punctuation>(new Punctuation(LESSER_THAN_EQUAL, this->pos.to_string()));
	}

	this->advance();
	return make_unique<Punctuation>(new Punctuation(LESSER_THAN, this->pos.to_string()));
}

unique_ptr<Punctuation> Lexer::consume_single_char_punctuation(char ch)
{
	this->pos.increment_column_number();
	this->advance();

	switch (ch)
	{
	case '\\':
	{
		return make_unique<Punctuation>(new Punctuation(BACKWARD_SLASH, this->pos.to_string()));
	}
	case ';':
	{
		return make_unique<Punctuation>(new Punctuation(SEMICOLON, this->pos.to_string()));
	}
	case '(':
	{
		return make_unique<Punctuation>(new Punctuation(OPEN_PARENTHESIS, this->pos.to_string()));
	}
	case ')':
	{
		return make_unique<Punctuation>(new Punctuation(CLOSE_PARENTHESIS, this->pos.to_string()));
	}
	case '{':
	{
		return make_unique<Punctuation>(new Punctuation(OPEN_CURLY_BRACE, this->pos.to_string()));
	}
	case '}':
	{
		return make_unique<Punctuation>(new Punctuation(CLOSE_CURLY_BRACE, this->pos.to_string()));
	}
	case '[':
	{
		return make_unique<Punctuation>(new Punctuation(OPEN_BRACKET, this->pos.to_string()));
	}
	case ']':
	{
		return make_unique<Punctuation>(new Punctuation(CLOSE_BRACKET, this->pos.to_string()));
	}
	case ',':
	{
		return make_unique<Punctuation>(new Punctuation(COMMA, this->pos.to_string()));
	}
	case '.':
	{
		return make_unique<Punctuation>(new Punctuation(DOT, this->pos.to_string()));
	}
	case ':':
	{
		return make_unique<Punctuation>(new Punctuation(COLON, this->pos.to_string()));
	}
	case '|':
	{
		return make_unique<Punctuation>(new Punctuation(BAR, this->pos.to_string()));
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

	return make_unique<Unknown>(new Unknown(unknown_token, this->pos.to_string()));
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
	if (index >= this->raw_source.size())
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