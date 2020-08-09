#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

#include "Token.h"
#include "TokenPosition.h"
#include "TokenType.h"
#include "Pointer.h"

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <map>

const std::map<std::wstring, TokenType> keyword_map =
{
	{ L"if", TokenType::IF },
	{ L"elif", TokenType::ELIF },
	{ L"else", TokenType::ELSE },

	{ L"and", TokenType::AND },
	{ L"or", TokenType::OR },

	{ L"let", TokenType::LET },
	{ L"const", TokenType::CONST_KEYWORD },

	{ L"while", TokenType::WHILE },
	{ L"for", TokenType::FOR },
	{ L"in", TokenType::IN_KEYWORD },
	{ L"break", TokenType::BREAK },
	{ L"continue", TokenType::CONTINUE },

	{ L"fn", TokenType::FN },
	{ L"return", TokenType::RETURN },
	{ L"gen", TokenType::GEN },
	{ L"yield", TokenType::YIELD_KEYWORD },

	{ L"type", TokenType::TYPE },
	{ L"num", TokenType::NUM },
	{ L"str", TokenType::STR },
	{ L"bool", TokenType::BOOL },
	{ L"enum", TokenType::ENUM },
	{ L"any", TokenType::ANY },
	{ L"opt", TokenType::OPT },

	{ L"none", TokenType::NONE },

	{ L"true", TokenType::TRUE_KEYWORD },
	{ L"false", TokenType::FALSE_KEYWORD },

	{ L"import", TokenType::IMPORT },
	{ L"from", TokenType::FROM },

	{ L"new", TokenType::NEW },

	{ L"pub", TokenType::PUB },
	{ L"pass", TokenType::PASS },

	{ L"assert", TokenType::ASSERT }
};

class LEXER_API Lexer
{
	std::wstring raw_source;

	TokenPosition position;
	Pointer pointer;

	std::vector<Token_ptr> tokens;

	// Consumers

	Token_ptr consume_number_literal(wchar_t ch);
	Token_ptr consume_identifier(wchar_t ch);
	Token_ptr consume_string_literal();

	Token_ptr consume_plus();
	Token_ptr consume_minus();
	Token_ptr consume_star();
	Token_ptr consume_division();
	Token_ptr consume_reminder();
	Token_ptr consume_power();
	Token_ptr consume_bang();
	Token_ptr consume_equal();
	Token_ptr consume_colon();
	Token_ptr consume_greater_than();
	Token_ptr consume_lesser_than();
	Token_ptr consume_single_char_punctuation(wchar_t ch);

	Token_ptr consume_eol();
	Token_ptr consume_space();

	Token_ptr consume_unknown_token(wchar_t ch);

	// Utils

	wchar_t get_char_at(int index) const;
	wchar_t get_current_char() const;
	wchar_t get_right_char() const;

	std::optional<Token_ptr> get_previous_significant_token();
	bool is_unary();

	bool expect_current_char(wchar_t ch);
	void next();
	void previous();

	void init(std::wstring raw_source);
public:
	Lexer() {};
	~Lexer()
	{
		raw_source = L"";
		tokens.clear();
	};

	std::vector<Token_ptr> execute(std::wstring raw_source);
};

using Lexer_ptr = LEXER_API std::unique_ptr<Lexer>;
