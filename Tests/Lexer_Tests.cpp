#pragma once
#include "pch.h"
#include "catch.hpp"
#include "test_utils.h"

#include "Token.h"
#include "TokenType.h"
#include "Lexer.h"

#include <string>
#include <vector>

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
			CREATE_EOL,
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
			CREATE_TRUE
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
			CREATE_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -5") {
		const string raw_source = "-5";

		vector<Token> expected_tokens = {
			CREATE_UNARY_MINUS,
			CREATE_NUMBER_LITERAL("5")
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
			CREATE_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +5") {
		const string raw_source = "+5";

		vector<Token> expected_tokens = {
			CREATE_UNARY_PLUS,
			CREATE_NUMBER_LITERAL("5")
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
			CREATE_CLOSE_PARENTHESIS
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
			CREATE_NUMBER_LITERAL("1")
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
			CREATE_CLOSE_PARENTHESIS
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
			CREATE_NUMBER_LITERAL("5")
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
			CREATE_OPEN_BRACKET,
			CREATE_NUM,
			CREATE_CLOSE_BRACKET,
			CREATE_EQUAL,
			CREATE_OPEN_BRACKET,
			CREATE_NUMBER_LITERAL("5"),
			CREATE_COMMA,
			CREATE_UNARY_MINUS,
			CREATE_NUMBER_LITERAL("6"),
			CREATE_COMMA,
			CREATE_NUMBER_LITERAL("7"),
			CREATE_CLOSE_BRACKET,
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
			CREATE_OPEN_BRACKET,
			CREATE_IDENTIFIER("Movie"),
			CREATE_CLOSE_BRACKET,

			CREATE_EQUAL,

			CREATE_OPEN_CURLY_BRACE,

			CREATE_IDENTIFIER("name"),
			CREATE_COLON,
			CREATE_STRING_LITERAL("Mad Max: Fury Road"),

			CREATE_COMMA,

			CREATE_IDENTIFIER("actors"),
			CREATE_COLON,

			CREATE_OPEN_BRACKET,
			CREATE_STRING_LITERAL("Tom Hardy"),
			CREATE_COMMA,
			CREATE_STRING_LITERAL("Charlize Theron"),
			CREATE_CLOSE_BRACKET,

			CREATE_CLOSE_CURLY_BRACE
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
			CREATE_NUMBER_LITERAL("5")
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
			CREATE_NUMBER_LITERAL("1")
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