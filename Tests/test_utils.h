#pragma once
#include "Token.h"
#include <vector>

#define LEXER_ACT										\
	Lexer lexer(raw_source);							\
	auto tokens = lexer.execute();						\
	auto actual_tokens = prepare_to_assert(tokens)

#define PARSER_ACT					\
	Parser parser(tokens);			\
	Module mod = parser.execute()

// Literals

#define CREATE_NUMBER_LITERAL(literal) Token { WTokenType::NumberLiteral, literal }
#define CREATE_STRING_LITERAL(literal) Token { WTokenType::StringLiteral, literal }

// Identifiers

#define CREATE_IDENTIFIER(identifier) Token { WTokenType::Identifier, identifier }
#define CREATE_FUNC_IDENTIFIER(identifier) Token { WTokenType::FunctionIdentifier, identifier }

// Punctuations

#define CREATE_OPEN_PARENTHESIS Token { WTokenType::OPEN_PARENTHESIS, "(" }
#define CREATE_CLOSE_PARENTHESIS Token { WTokenType::CLOSE_PARENTHESIS, ")" }
#define CREATE_OPEN_CURLY_BRACE Token { WTokenType::OPEN_CURLY_BRACE, "{" }
#define CREATE_CLOSE_CURLY_BRACE Token { WTokenType::CLOSE_CURLY_BRACE, "}" }
#define CREATE_OPEN_BRACKET Token { WTokenType::OPEN_BRACKET, "[" }
#define CREATE_CLOSE_BRACKET Token { WTokenType::CLOSE_BRACKET, "]" }

#define CREATE_COMMA Token { WTokenType::COMMA, "," }
#define CREATE_DOT Token { WTokenType::DOT, "." }
#define CREATE_COLON Token { WTokenType::COLON, ":" }
#define CREATE_ARROW Token { WTokenType::ARROW, "->" }

#define CREATE_EQUAL Token { WTokenType::EQUAL, "=" }
#define CREATE_BANG_EQUAL Token { WTokenType::BANG_EQUAL, "!=" }

#define CREATE_BANG Token { WTokenType::BANG, "!" }
#define CREATE_PLUS Token { WTokenType::PLUS, "+" }
#define CREATE_UNARY_PLUS Token { WTokenType::UNARY_PLUS, "+" }
#define CREATE_MINUS Token { WTokenType::MINUS, "-" }
#define CREATE_UNARY_MINUS Token { WTokenType::UNARY_MINUS, "-" }
#define CREATE_STAR Token { WTokenType::STAR, "*" }
#define CREATE_DIVISION Token { WTokenType::DIVISION, "/" }
#define CREATE_REMINDER Token { WTokenType::REMINDER, "%" }
#define CREATE_POWER Token { WTokenType::POWER, "^" }

#define CREATE_LESSER_THAN Token { WTokenType::LESSER_THAN, "<" }
#define CREATE_LESSER_THAN_EQUAL Token { WTokenType::LESSER_THAN_EQUAL, "<=" }

#define CREATE_GREATER_THAN Token { WTokenType::GREATER_THAN, ">" }
#define CREATE_GREATER_THAN_EQUAL Token { WTokenType::GREATER_THAN_EQUAL, ">=" }

// Keywords

#define CREATE_IF Token { WTokenType::IF, "if" }
#define CREATE_ELSE Token { WTokenType::ELSE, "else" }

#define CREATE_AND Token { WTokenType::AND, "and" }
#define CREATE_OR Token { WTokenType::OR, "or" }

#define CREATE_CONST Token { WTokenType::CONST_KEYWORD, "const" }
#define CREATE_LET Token { WTokenType::LET, "let" }

#define CREATE_LOOP Token { WTokenType::LOOP, "loop" }
#define CREATE_FOR Token { WTokenType::FOR, "for" }
#define CREATE_BREAK Token { WTokenType::BREAK, "break" }
#define CREATE_CONTINUE Token { WTokenType::CONTINUE, "continue" }
#define CREATE_IN Token { WTokenType::IN_KEYWORD, "in" }

#define CREATE_FN Token { WTokenType::FN, "fn" }
#define CREATE_RETURN Token { WTokenType::RETURN, "return" }

#define CREATE_NUM Token { WTokenType::NUM, "num" }
#define CREATE_STR Token { WTokenType::STR, "str" }
#define CREATE_BOOL Token { WTokenType::BOOL, "bool" }
#define CREATE_ENUM Token { WTokenType::ENUM, "enum" }
#define CREATE_TYPE Token { WTokenType::TYPE, "type" }
#define CREATE_OPT Token { WTokenType::OPT, "opt" }

#define CREATE_TRUE Token { WTokenType::TRUE_KEYWORD, "true" }
#define CREATE_FALSE Token { WTokenType::FALSE_KEYWORD, "false" }

#define CREATE_IMPORT Token { WTokenType::IMPORT, "import" }
#define CREATE_FROM Token { WTokenType::FROM, "from" }
#define CREATE_PUB Token { WTokenType::PUB, "pub" }

// Other

#define CREATE_EOL Token { WTokenType::EOL, "\\n" }
#define CREATE_UNKNOWN Token { WTokenType::UNKNOWN, "" }

// Functions

std::vector<Token> prepare_to_assert(std::vector<Token_ptr> vector_of_pointers);
