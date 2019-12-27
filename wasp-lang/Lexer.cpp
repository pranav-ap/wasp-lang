#pragma once
#include <map>
#include "Lexer.h"

using namespace std;

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
	{ "pure", TokenType::PURE }
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
			this->pointer.advance();
			this->position.increment_column_number();
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
		else if (ch == '\n') {
			token = this->consume_eol();
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
			this->position.increment_column_number();
			this->pointer.advance();
			continue;
		}

		break;
	}

	this->position.increment_column_number();
	this->pointer.advance();

	return make_unique<Token>(TokenType::NumberLiteral, number_literal, this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::consume_string_literal()
{
	string string_literal;

	this->position.increment_column_number();
	this->pointer.advance();

	while (char ch = this->get_current_char())
	{
		if (ch == '"')
		{
			break;
		}

		string_literal.push_back(ch);
		this->position.increment_column_number();
		this->pointer.advance();
	}

	this->position.increment_column_number();
	this->pointer.advance();

	return make_unique<Token>(TokenType::StringLiteral, string_literal, this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::consume_identifier(char ch)
{
	string identifier;
	identifier.push_back(ch);

	this->pointer.advance();
	this->position.increment_column_number();

	while (ch = this->get_current_char())
	{
		if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '_' || std::isalpha(ch))
		{
			identifier.push_back(ch);
			this->position.increment_column_number();
			this->pointer.advance();
			continue;
		}

		break;
	}

	if (keyword_map.count(identifier) > 0)
	{
		TokenType keyword_type = keyword_map[identifier];
		return make_unique<Token>(keyword_type, identifier, this->position.get_line_num(), this->position.get_column_num());
	}

	if (this->get_current_char() == '(') {
		return make_unique<Token>(TokenType::FunctionIdentifier, identifier, this->position.get_line_num(), this->position.get_column_num());
	}

	return make_unique<Token>(TokenType::Identifier, identifier, this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_plus()
{
	if (this->peek_and_move('='))
	{
		this->pointer.advance();
		this->position.increment_column_number();
		return make_unique<Token>(TokenType::PLUS_EQUAL, "+=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->pointer.advance();
	this->position.increment_column_number();
	return make_unique<Token>(TokenType::PLUS, "+", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_minus()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::MINUS_EQUAL, "-=", this->position.get_line_num(), this->position.get_column_num());
	}
	else if (this->peek_and_move('>'))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::ARROW, "->", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::MINUS, "-", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_star()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::STAR_EQUAL, "*=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::STAR, "*", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_division()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::DIVISION_EQUAL, "/=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::DIVISION, "/", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_reminder()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::REMINDER_EQUAL, "%=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::REMINDER, "%", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_power()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::POWER_EQUAL, "^=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::POWER, "^", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_bang()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::BANG_EQUAL, "!=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::BANG, "!", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_equal()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::EQUAL_EQUAL, "==", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::EQUAL, "=", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_greater_than()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::GREATER_THAN_EQUAL, ">=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::GREATER_THAN, ">", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::handle_lesser_than()
{
	if (this->peek_and_move('='))
	{
		this->position.increment_column_number();
		this->pointer.advance();
		return make_unique<Token>(TokenType::LESSER_THAN_EQUAL, "<=", this->position.get_line_num(), this->position.get_column_num());
	}

	this->position.increment_column_number();
	this->pointer.advance();
	return make_unique<Token>(TokenType::LESSER_THAN, "<", this->position.get_line_num(), this->position.get_column_num());
}

shared_ptr<Token> Lexer::consume_single_char_punctuation(char ch)
{
	this->position.increment_column_number();
	this->pointer.advance();

	switch (ch)
	{
	case '\\':
	{
		return make_unique<Token>(TokenType::BACKWARD_SLASH, "\\", this->position.get_line_num(), this->position.get_column_num());
	}
	case '(':
	{
		return make_unique<Token>(TokenType::OPEN_PARENTHESIS, "(", this->position.get_line_num(), this->position.get_column_num());
	}
	case ')':
	{
		return make_unique<Token>(TokenType::CLOSE_PARENTHESIS, ")", this->position.get_line_num(), this->position.get_column_num());
	}
	case '{':
	{
		return make_unique<Token>(TokenType::OPEN_CURLY_BRACE, "{", this->position.get_line_num(), this->position.get_column_num());
	}
	case '}':
	{
		return make_unique<Token>(TokenType::CLOSE_CURLY_BRACE, "}", this->position.get_line_num(), this->position.get_column_num());
	}
	case '[':
	{
		return make_unique<Token>(TokenType::OPEN_BRACKET, "[", this->position.get_line_num(), this->position.get_column_num());
	}
	case ']':
	{
		return make_unique<Token>(TokenType::CLOSE_BRACKET, "]", this->position.get_line_num(), this->position.get_column_num());
	}
	case ',':
	{
		return make_unique<Token>(TokenType::COMMA, ",", this->position.get_line_num(), this->position.get_column_num());
	}
	case '.':
	{
		return make_unique<Token>(TokenType::DOT, ".", this->position.get_line_num(), this->position.get_column_num());
	}
	case ':':
	{
		return make_unique<Token>(TokenType::COLON, ":", this->position.get_line_num(), this->position.get_column_num());
	}
	case '|':
	{
		return make_unique<Token>(TokenType::BAR, "|", this->position.get_line_num(), this->position.get_column_num());
	}
	default:
		return nullptr;
	}
}

shared_ptr<Token> Lexer::consume_eol()
{
	int line_num = this->position.get_line_num();;
	int column_num = this->position.get_column_num() + 1;

	this->pointer.advance();
	this->position.increment_line_number();
	this->position.reset_column_number();

	return make_unique<Token>(TokenType::EOL, "\\n", line_num, column_num);
}

shared_ptr<Token> Lexer::consume_unknown_token(char ch)
{
	string unknown_token;
	unknown_token.push_back(ch);

	while (char ch = this->get_right_char())
	{
		if (ch != ' ' && ch != '\n')
		{
			unknown_token.push_back(ch);
			this->position.increment_column_number();
			this->pointer.advance();
			continue;
		}

		break;
	}

	return make_unique<Token>(TokenType::UNKNOWN, unknown_token, this->position.get_line_num(), this->position.get_column_num());
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