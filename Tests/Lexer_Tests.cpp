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

SCENARIO("Scanning Unary Expressions", "[Lexer]") {
	GIVEN("Bang Operator : !true") {
		const string raw_source = "!true";

		vector<Token> expected_tokens = {
			EXPECT_BANG,
			EXPECT_TRUE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Bang Operator : !(true and false)") {
		const string raw_source = "!(true and false)";

		vector<Token> expected_tokens = {
			EXPECT_BANG,
			EXPECT_OPEN_PARENTHESIS,
			EXPECT_TRUE,
			EXPECT_AND,
			EXPECT_FALSE,
			EXPECT_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -5") {
		const string raw_source = "-5";

		vector<Token> expected_tokens = {
			EXPECT_UNARY_MINUS,
			EXPECT_NUMBER_LITERAL("5")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -(5 + 5)") {
		const string raw_source = "-(5 + 5)";

		vector<Token> expected_tokens = {
			EXPECT_UNARY_MINUS,
			EXPECT_OPEN_PARENTHESIS,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_PLUS,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +5") {
		const string raw_source = "+5";

		vector<Token> expected_tokens = {
			EXPECT_UNARY_PLUS,
			EXPECT_NUMBER_LITERAL("5")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +(5 + 5)") {
		const string raw_source = "+(5 + 5)";

		vector<Token> expected_tokens = {
			EXPECT_UNARY_PLUS,
			EXPECT_OPEN_PARENTHESIS,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_PLUS,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Scanning Binary Expressions", "[Lexer]") {
	GIVEN("5 + 1") {
		const string raw_source = "5 + 1";

		vector<Token> expected_tokens = {
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_PLUS,
			EXPECT_NUMBER_LITERAL("1")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("5 + 1 / 3 + (3 - 1)") {
		const string raw_source = "5 + 1 / 3 + (3 - 1)";

		vector<Token> expected_tokens = {
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_PLUS,
			EXPECT_NUMBER_LITERAL("1"),
			EXPECT_DIVISION,
			EXPECT_NUMBER_LITERAL("3"),
			EXPECT_PLUS,
			EXPECT_OPEN_PARENTHESIS,
			EXPECT_NUMBER_LITERAL("3"),
			EXPECT_MINUS,
			EXPECT_NUMBER_LITERAL("1"),
			EXPECT_CLOSE_PARENTHESIS
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Declaring Variables", "[Lexer]") {
	GIVEN("let a : num = 5") {
		const string raw_source = "let a : num = 5";

		vector<Token> expected_tokens = {
			EXPECT_LET,
			EXPECT_IDENTIFIER("a"),
			EXPECT_COLON,
			EXPECT_NUM,
			EXPECT_EQUAL,
			EXPECT_NUMBER_LITERAL("5")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("let a : [num] = [5, -6, 7]") {
		const string raw_source = "let a : [num] = [5, -6, 7]";

		vector<Token> expected_tokens = {
			EXPECT_LET,
			EXPECT_IDENTIFIER("a"),
			EXPECT_COLON,
			EXPECT_OPEN_BRACKET,
			EXPECT_NUM,
			EXPECT_CLOSE_BRACKET,
			EXPECT_EQUAL,
			EXPECT_OPEN_BRACKET,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_COMMA,
			EXPECT_UNARY_MINUS,
			EXPECT_NUMBER_LITERAL("6"),
			EXPECT_COMMA,
			EXPECT_NUMBER_LITERAL("7"),
			EXPECT_CLOSE_BRACKET,
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
			EXPECT_LET,
			EXPECT_IDENTIFIER("a"),
			EXPECT_COLON,
			EXPECT_OPEN_BRACKET,
			EXPECT_IDENTIFIER("Movie"),
			EXPECT_CLOSE_BRACKET,

			EXPECT_EQUAL,

			EXPECT_OPEN_CURLY_BRACE,

			EXPECT_IDENTIFIER("name"),
			EXPECT_COLON,
			EXPECT_STRING_LITERAL("Mad Max: Fury Road"),

			EXPECT_COMMA,

			EXPECT_IDENTIFIER("actors"),
			EXPECT_COLON,

			EXPECT_OPEN_BRACKET,
			EXPECT_STRING_LITERAL("Tom Hardy"),
			EXPECT_COMMA,
			EXPECT_STRING_LITERAL("Charlize Theron"),
			EXPECT_CLOSE_BRACKET,

			EXPECT_CLOSE_CURLY_BRACE
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Assignments", "[Lexer]") {
	GIVEN("a = 5") {
		const string raw_source = "a = 5";

		vector<Token> expected_tokens = {
			EXPECT_IDENTIFIER("a"),
			EXPECT_EQUAL,
			EXPECT_NUMBER_LITERAL("5")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("a = 5 * 2 + 1") {
		const string raw_source = "a = 5 * 2 + 1";

		vector<Token> expected_tokens = {
			EXPECT_IDENTIFIER("a"),
			EXPECT_EQUAL,
			EXPECT_NUMBER_LITERAL("5"),
			EXPECT_STAR,
			EXPECT_NUMBER_LITERAL("2"),
			EXPECT_PLUS,
			EXPECT_NUMBER_LITERAL("1")
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}