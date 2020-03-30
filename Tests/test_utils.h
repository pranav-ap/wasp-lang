#pragma once
#include "Token.h"
#include <vector>

#define LEXER_ACT										\
	Lexer lexer(raw_source);							\
	auto tokens = lexer.execute();						\
	auto actual_tokens = compose_tokens(tokens)

// Literals

#define EXPECT_NUMBER_LITERAL(literal) Token { WTokenType::NumberLiteral, literal }
#define EXPECT_STRING_LITERAL(literal) Token { WTokenType::StringLiteral, literal }

// Identifiers

#define EXPECT_IDENTIFIER(identifier) Token { WTokenType::Identifier, identifier }
#define EXPECT_FUNC_IDENTIFIER(identifier) Token { WTokenType::FunctionIdentifier, identifier }

// Punctuations

#define EXPECT_OPEN_PARENTHESIS Token { WTokenType::OPEN_PARENTHESIS, "(" }
#define EXPECT_CLOSE_PARENTHESIS Token { WTokenType::CLOSE_PARENTHESIS, ")" }
#define EXPECT_OPEN_CURLY_BRACE Token { WTokenType::OPEN_CURLY_BRACE, "{" }
#define EXPECT_CLOSE_CURLY_BRACE Token { WTokenType::CLOSE_CURLY_BRACE, "}" }
#define EXPECT_OPEN_BRACKET Token { WTokenType::OPEN_BRACKET, "[" }
#define EXPECT_CLOSE_BRACKET Token { WTokenType::CLOSE_BRACKET, "]" }

#define EXPECT_COMMA Token { WTokenType::COMMA, "," }
#define EXPECT_DOT Token { WTokenType::DOT, "." }
#define EXPECT_COLON Token { WTokenType::COLON, ":" }
#define EXPECT_ARROW Token { WTokenType::ARROW, "->" }

#define EXPECT_EQUAL Token { WTokenType::EQUAL, "=" }
#define EXPECT_BANG_EQUAL Token { WTokenType::BANG_EQUAL, "!=" }

#define EXPECT_BANG Token { WTokenType::BANG, "!" }
#define EXPECT_PLUS Token { WTokenType::PLUS, "+" }
#define EXPECT_UNARY_PLUS Token { WTokenType::UNARY_PLUS, "+" }
#define EXPECT_MINUS Token { WTokenType::MINUS, "-" }
#define EXPECT_UNARY_MINUS Token { WTokenType::UNARY_MINUS, "-" }
#define EXPECT_STAR Token { WTokenType::STAR, "*" }
#define EXPECT_DIVISION Token { WTokenType::DIVISION, "/" }
#define EXPECT_REMINDER Token { WTokenType::REMINDER, "%" }
#define EXPECT_POWER Token { WTokenType::POWER, "^" }

#define EXPECT_LESSER_THAN Token { WTokenType::LESSER_THAN, "<" }
#define EXPECT_LESSER_THAN_EQUAL Token { WTokenType::LESSER_THAN_EQUAL, "<=" }

#define EXPECT_GREATER_THAN Token { WTokenType::GREATER_THAN, ">" }
#define EXPECT_GREATER_THAN_EQUAL Token { WTokenType::GREATER_THAN_EQUAL, ">=" }

// Keywords

#define EXPECT_IF Token { WTokenType::IF, "if" }
#define EXPECT_ELSE Token { WTokenType::ELSE, "else" }

#define EXPECT_AND Token { WTokenType::AND, "and" }
#define EXPECT_OR Token { WTokenType::OR, "or" }

#define EXPECT_CONST Token { WTokenType::CONST_KEYWORD, "const" }
#define EXPECT_LET Token { WTokenType::LET, "let" }

#define EXPECT_LOOP Token { WTokenType::LOOP, "loop" }
#define EXPECT_FOR Token { WTokenType::FOR, "for" }
#define EXPECT_BREAK Token { WTokenType::BREAK, "break" }
#define EXPECT_CONTINUE Token { WTokenType::CONTINUE, "continue" }
#define EXPECT_IN Token { WTokenType::IN_KEYWORD, "in" }

#define EXPECT_FN Token { WTokenType::FN, "fn" }
#define EXPECT_RETURN Token { WTokenType::RETURN, "return" }

#define EXPECT_NUM Token { WTokenType::NUM, "num" }
#define EXPECT_STR Token { WTokenType::STR, "str" }
#define EXPECT_BOOL Token { WTokenType::BOOL, "bool" }
#define EXPECT_ENUM Token { WTokenType::ENUM, "enum" }
#define EXPECT_TYPE Token { WTokenType::TYPE, "type" }
#define EXPECT_OPT Token { WTokenType::OPT, "opt" }

#define EXPECT_TRUE Token { WTokenType::TRUE_KEYWORD, "true" }
#define EXPECT_FALSE Token { WTokenType::FALSE_KEYWORD, "false" }

#define EXPECT_IMPORT Token { WTokenType::IMPORT, "import" }
#define EXPECT_FROM Token { WTokenType::FROM, "from" }
#define EXPECT_PUB Token { WTokenType::PUB, "pub" }

// Other

#define EXPECT_EOL Token { WTokenType::EOL, "\\n" }
#define EXPECT_UNKNOWN Token { WTokenType::UNKNOWN, "" }

// Functions

std::vector<Token> compose_tokens(std::vector<Token_ptr> vector_of_pointers);
