#pragma once
#include "pch.h"
#include "catch.hpp"
#include "test_utils.h"

#include "Token.h"
#include "TokenType.h"
#include "Lexer.h"

#include <string>
#include <vector>

// ------ TOKEN MACROS ------

// Literals

#define CREATE_NUMBER_LITERAL(x) Token { WTokenType::NumberLiteral, x }
#define CREATE_STRING_LITERAL(x) Token { WTokenType::StringLiteral, x }

// Identifiers

#define CREATE_IDENTIFIER(x) Token { WTokenType::Identifier, x }
#define CREATE_CALLABLE_IDENTIFIER(x) Token { WTokenType::CallableIdentifier, x }

// Punctuations

#define CREATE_OPEN_PARENTHESIS Token { WTokenType::OPEN_PARENTHESIS, "(" }
#define CREATE_CLOSE_PARENTHESIS Token { WTokenType::CLOSE_PARENTHESIS, ")" }
#define CREATE_OPEN_CURLY_BRACE Token { WTokenType::OPEN_CURLY_BRACE, "{" }
#define CREATE_CLOSE_CURLY_BRACE Token { WTokenType::CLOSE_CURLY_BRACE, "}" }
#define CREATE_OPEN_SQUARE_BRACKET Token { WTokenType::OPEN_SQUARE_BRACKET, "[" }
#define CREATE_CLOSE_SQUARE_BRACKET Token { WTokenType::CLOSE_SQUARE_BRACKET, "]" }
#define CREATE_OPEN_TUPLE_PARENTHESIS Token { WTokenType::OPEN_TUPLE_PARENTHESIS, "(@" }

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
#define CREATE_IN Token { WTokenType::IN_KEYWORD, "in" }
#define CREATE_BREAK Token { WTokenType::BREAK, "break" }
#define CREATE_CONTINUE Token { WTokenType::CONTINUE, "continue" }

#define CREATE_FN Token { WTokenType::FN, "fn" }
#define CREATE_RETURN Token { WTokenType::RETURN, "return" }
#define CREATE_GEN Token { WTokenType::GEN, "gen" }
#define CREATE_YIELD Token { WTokenType::YIELD, "yield" }

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

#define CREATE_SPACE Token { WTokenType::SPACE, " " }
#define CREATE_TAB Token { WTokenType::SPACE, " " }, Token { WTokenType::SPACE, " " }, Token { WTokenType::SPACE, " " }, Token { WTokenType::SPACE, " " }
#define CREATE_EOL Token { WTokenType::EOL, "\\n" }
#define CREATE_UNKNOWN Token { WTokenType::UNKNOWN, "" }

using std::string;
using std::vector;
using Catch::Matchers::Equals;

SCENARIO("Dealing with redundant EOLs", "[Lexer]") {
	GIVEN("An Empty File As Input") {
		const string raw_source =
			"\n"
			"\n"
			"\n"
			"\n"
			"\n";

		vector<Token> expected_tokens = {
			CREATE_EOL,
			CREATE_EOL,
			CREATE_EOL,
			CREATE_EOL,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Multiple EOLs before a statement") {
		const string raw_source =
			"\n"
			"\n"
			"\n"
			"\n"
			"a = 4 \n"
			"\n"
			"\n";

		vector<Token> expected_tokens = {
			CREATE_EOL,
			CREATE_EOL,
			CREATE_EOL,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("4"),
			CREATE_EOL,

			CREATE_EOL,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Scanning Unary Expressions", "[Lexer]") {
	GIVEN("Bang Operator : !true") {
		const string raw_source = "!true";

		vector<Token> expected_tokens = {
			CREATE_BANG,
			CREATE_TRUE,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Bang Operator : !(true and false)") {
		const string raw_source = "!(true and false)";

		vector<Token> expected_tokens = {
			CREATE_BANG,
			CREATE_OPEN_PARENTHESIS,
			CREATE_TRUE,
			CREATE_AND,
			CREATE_FALSE,
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -5") {
		const string raw_source = "-5";

		vector<Token> expected_tokens = {
			CREATE_UNARY_MINUS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -(5 + 5)") {
		const string raw_source = "-(5 + 5)";

		vector<Token> expected_tokens = {
			CREATE_UNARY_MINUS,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +5") {
		const string raw_source = "+5";

		vector<Token> expected_tokens = {
			CREATE_UNARY_PLUS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +(5 + 5)") {
		const string raw_source = "+(5 + 5)";

		vector<Token> expected_tokens = {
			CREATE_UNARY_PLUS,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Scanning Binary Expressions", "[Lexer]") {
	GIVEN("5 + 1") {
		const string raw_source = "5 + 1";

		vector<Token> expected_tokens = {
			CREATE_NUMBER_LITERAL("5"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("5 + 1 / 3 + (3 - 1)") {
		const string raw_source = "5 + 1 / 3 + (3 - 1)";

		vector<Token> expected_tokens = {
			CREATE_NUMBER_LITERAL("5"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_DIVISION,
			CREATE_NUMBER_LITERAL("3"),
			CREATE_PLUS,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("3"),
			CREATE_MINUS,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Declaring Variables", "[Lexer]") {
	GIVEN("let a : num = 5") {
		const string raw_source = "let a : num = 5";

		vector<Token> expected_tokens = {
			CREATE_LET,
			CREATE_IDENTIFIER("a"),
			CREATE_COLON,
			CREATE_NUM,
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("let a : [num] = [5, -6, 7]") {
		const string raw_source = "let a : [num] = [5, -6, 7]";

		vector<Token> expected_tokens = {
			CREATE_LET,
			CREATE_IDENTIFIER("a"),
			CREATE_COLON,
			CREATE_OPEN_SQUARE_BRACKET,
			CREATE_NUM,
			CREATE_CLOSE_SQUARE_BRACKET,
			CREATE_EQUAL,
			CREATE_OPEN_SQUARE_BRACKET,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_COMMA,
			CREATE_UNARY_MINUS,
			CREATE_NUMBER_LITERAL("6"),
			CREATE_COMMA,
			CREATE_NUMBER_LITERAL("7"),
			CREATE_CLOSE_SQUARE_BRACKET,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("let a : [Movie] = { ... }") {
		const string raw_source =
			"let a : [Movie] = {"
			"  name: \"Mad Max: Fury Road\","
			"  actors : [ \"Tom Hardy\", \"Charlize Theron\"]"
			"}";

		vector<Token> expected_tokens = {
			CREATE_LET,
			CREATE_IDENTIFIER("a"),
			CREATE_COLON,
			CREATE_OPEN_SQUARE_BRACKET,
			CREATE_IDENTIFIER("Movie"),
			CREATE_CLOSE_SQUARE_BRACKET,

			CREATE_EQUAL,

			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("name"),
			CREATE_COLON,
			CREATE_STRING_LITERAL("Mad Max: Fury Road"),

			CREATE_COMMA,
			CREATE_EOL,

			CREATE_IDENTIFIER("actors"),
			CREATE_COLON,

			CREATE_OPEN_SQUARE_BRACKET,
			CREATE_STRING_LITERAL("Tom Hardy"),
			CREATE_COMMA,
			CREATE_STRING_LITERAL("Charlize Theron"),
			CREATE_CLOSE_SQUARE_BRACKET,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Assignments", "[Lexer]") {
	GIVEN("a = 5") {
		const string raw_source = "a = 5";

		vector<Token> expected_tokens = {
			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("a = 5 * 2 + 1") {
		const string raw_source = "a = 5 * 2 + 1";

		vector<Token> expected_tokens = {
			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("2"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Branching", "[Lexer]") {
	GIVEN("IF with simple condition") {
		const string raw_source =
			"if true { \n"
			"    a = 1 \n"
			"    b = a + 43 * 5 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("IF with complex condition") {
		const string raw_source =
			"if true and 34 > 3 { \n"
			"    a = 1 \n"
			"    b = a + 43 * 5 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_AND,
			CREATE_NUMBER_LITERAL("34"),
			CREATE_GREATER_THAN,
			CREATE_NUMBER_LITERAL("3"),
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("IF-ELSE with simple condition") {
		const string raw_source =
			"if true { \n"
			"    a = 1 \n"
			"} else {\n"
			"    b = a + 43 * 5 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("IF-ELSE-IF") {
		const string raw_source =
			"if true { \n"
			"    a = 1 \n"
			"} else if (54 + 23 > 87) and !true {\n"
			"    b = a + 43 * 5 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_IF,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("54"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("23"),
			CREATE_GREATER_THAN,
			CREATE_NUMBER_LITERAL("87"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_AND,
			CREATE_BANG,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("IF-ELSE-IF-ELSE") {
		const string raw_source =
			"if true { \n"
			"    a = 1 \n"
			"    b = 32 \n"
			"} else if (54 + 23 > 87) and !true {\n"
			"    b = a + 43 * 5 \n"
			"} else { \n"
			"    c = a + b * (78 + (3 % 23)) \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("32"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_IF,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("54"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("23"),
			CREATE_GREATER_THAN,
			CREATE_NUMBER_LITERAL("87"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_AND,
			CREATE_BANG,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("c"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_IDENTIFIER("b"),
			CREATE_STAR,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("78"),
			CREATE_PLUS,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("3"),
			CREATE_REMINDER,
			CREATE_NUMBER_LITERAL("23"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("LET inside a IF block") {
		const string raw_source =
			"if true { \n"
			"    let a : num  = 1 \n"
			"    b = a + 43 * 5 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_LET,
			CREATE_IDENTIFIER("a"),
			CREATE_COLON,
			CREATE_NUM,
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("43"),
			CREATE_STAR,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Loop", "[Lexer]") {
	GIVEN("A Loop without blocks") {
		const string raw_source =
			"loop { \n"
			"    a = 1 \n"
			"    b = 32 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_LOOP,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("32"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("A Loop with a IF-ELSE-IF-ELSE") {
		const string raw_source =
			"loop { \n"
			"    a = 1 \n"
			"    b = 32 \n"
			"\n"
			"    if true { \n"
			"        a = 1 \n"
			"        b = 32 \n"
			"    } else if (54 + 23 > 87) and !true {\n"
			"        break \n"
			"    } else { \n"
			"        c = a + b * (78 + (3 % 23)) \n"
			"        continue \n"
			"    } \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_LOOP,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("32"),
			CREATE_EOL,

			CREATE_EOL,

			CREATE_IF,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("32"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_IF,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("54"),
			CREATE_PLUS,
			CREATE_NUMBER_LITERAL("23"),
			CREATE_GREATER_THAN,
			CREATE_NUMBER_LITERAL("87"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_AND,
			CREATE_BANG,
			CREATE_TRUE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_BREAK,
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_ELSE,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("c"),
			CREATE_EQUAL,
			CREATE_IDENTIFIER("a"),
			CREATE_PLUS,
			CREATE_IDENTIFIER("b"),
			CREATE_STAR,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("78"),
			CREATE_PLUS,
			CREATE_OPEN_PARENTHESIS,
			CREATE_NUMBER_LITERAL("3"),
			CREATE_REMINDER,
			CREATE_NUMBER_LITERAL("23"),
			CREATE_CLOSE_PARENTHESIS,
			CREATE_CLOSE_PARENTHESIS,
			CREATE_EOL,

			CREATE_CONTINUE,
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE,
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("LET inside a Loop block") {
		const string raw_source =
			"loop { \n"
			"    a = 1 \n"
			"    b = 32 \n"
			"}";

		vector<Token> expected_tokens = {
			CREATE_LOOP,
			CREATE_OPEN_CURLY_BRACE,
			CREATE_EOL,

			CREATE_IDENTIFIER("a"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("1"),
			CREATE_EOL,

			CREATE_IDENTIFIER("b"),
			CREATE_EQUAL,
			CREATE_NUMBER_LITERAL("32"),
			CREATE_EOL,

			CREATE_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}