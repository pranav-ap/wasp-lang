#include "Lexer.h"

using namespace std;

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

			if (token != nullptr) {
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

unique_ptr<TokenType> Lexer::consume_number_literal(char ch)
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

	TokenType token = NumberLiteral(number, this->pos.to_string());

	return make_unique<TokenType>(token);
}

unique_ptr<TokenType> Lexer::consume_string_literal()
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

	TokenType token = StringLiteral(string_literal, this->pos.to_string());

	return make_unique<TokenType>(token);
}

unique_ptr<TokenType> Lexer::consume_identifier(char ch)
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
		return make_unique<TokenType>(keyword_type, this->pos.to_string());
	}

	if (this->get_right_char() == '(')
	{
		return make_unique<TokenType>(identifier, this->pos.to_string());
	}

	return make_unique<TokenType>(identifier, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_plus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::PLUS_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::PLUS, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_minus()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::MINUS_EQUAL, this->pos.to_string());
	}
	else if (this->peek_and_move('>'))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::ARROW, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::MINUS, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_star()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::STAR_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::STAR, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_division()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::DIVISION_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::DIVISION, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_reminder()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::REMINDER_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::REMINDER, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_power()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::POWER_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::POWER, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_bang()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::BANG_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::BANG, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_equal()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::EQUAL_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::EQUAL, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_greater_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::GREATER_THAN_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::GREATER_THAN, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::handle_lesser_than()
{
	if (this->peek_and_move('='))
	{
		this->pos.increment_column_number();
		this->advance();
		return make_unique<TokenType>(PunctuationType::LESSER_THAN_EQUAL, this->pos.to_string());
	}

	this->advance();
	return make_unique<TokenType>(PunctuationType::LESSER_THAN, this->pos.to_string());
}

unique_ptr<TokenType> Lexer::consume_single_char_punctuation(char ch)
{
	this->pos.increment_column_number();
	this->advance();

	switch (ch)
	{
	case '\\':
	{
		return make_unique<TokenType>(PunctuationType::BACKWARD_SLASH, this->pos.to_string());
	}
	case ';':
	{
		return make_unique<TokenType>(PunctuationType::SEMICOLON, this->pos.to_string());
	}
	case '(':
	{
		return make_unique<TokenType>(PunctuationType::OPEN_PARENTHESIS, this->pos.to_string());
	}
	case ')':
	{
		return make_unique<TokenType>(PunctuationType::CLOSE_PARENTHESIS, this->pos.to_string());
	}
	case '{':
	{
		return make_unique<TokenType>(PunctuationType::OPEN_CURLY_BRACE, this->pos.to_string());
	}
	case '}':
	{
		return make_unique<TokenType>(PunctuationType::CLOSE_CURLY_BRACE, this->pos.to_string());
	}
	case '[':
	{
		return make_unique<TokenType>(PunctuationType::OPEN_BRACKET, this->pos.to_string());
	}
	case ']':
	{
		return make_unique<TokenType>(PunctuationType::CLOSE_BRACKET, this->pos.to_string());
	}
	case ',':
	{
		return make_unique<TokenType>(PunctuationType::COMMA, this->pos.to_string());
	}
	case '.':
	{
		return make_unique<TokenType>(PunctuationType::DOT, this->pos.to_string());
	}
	case ':':
	{
		return make_unique<TokenType>(PunctuationType::COLON, this->pos.to_string());
	}
	case '|':
	{
		return make_unique<TokenType>(PunctuationType::BAR, this->pos.to_string());
	}
	}
}

unique_ptr<TokenType> Lexer::consume_unknown_token(char ch)
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

	return make_unique<TokenType>(unknown_token, this->pos.to_string());
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