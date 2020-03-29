#pragma once
#include "pch.h"
#include "catch.hpp"
#include "test_utils.h"

#include "Token.h"
#include "TokenType.h"
#include "Lexer.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using Catch::Matchers::Equals;

#define LEXER_ACT										\
	Lexer lexer(raw_source);							\
	auto tokens = lexer.execute();						\
	auto actual_tokens = compose_tokens(tokens)

SCENARIO("Scanning Unary Expressions", "[Lexer]") {
	GIVEN("Bang Operator : !true") {
		const string raw_source = "!true";

		vector<Token> expected_tokens = {
			Token { WTokenType::BANG, "!" },
			Token { WTokenType::TRUE_KEYWORD, "true" }
		};

		LEXER_ACT;
	}

	GIVEN("Bang Operator : !(true and false)") {
		const string raw_source = "!(true and false)";

		vector<Token> expected_tokens = {
			Token { WTokenType::BANG, "!" },
			Token { WTokenType::OPEN_PARENTHESIS, "(" },
			Token { WTokenType::TRUE_KEYWORD, "true" },
			Token { WTokenType::AND, "and" },
			Token { WTokenType::FALSE_KEYWORD, "false" },
			Token { WTokenType::CLOSE_PARENTHESIS, ")" }
		};

		LEXER_ACT;
	}

	GIVEN("Unary minus : -5") {
		const string raw_source = "-5";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_MINUS, "-" },
			Token { WTokenType::NumberLiteral, "5" }
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -5 + 5") {
		const string raw_source = "-5 + 5";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_MINUS, "-" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" }
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary minus : -(5 + 5)") {
		const string raw_source = "-(5 + 5)";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_MINUS, "-" },
			Token { WTokenType::OPEN_PARENTHESIS, "(" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::CLOSE_PARENTHESIS, ")" },
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +5") {
		const string raw_source = "+5";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" }
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +5 + 5") {
		const string raw_source = "+5 + 5";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" }
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}

	GIVEN("Unary plus : +(5 + 5)") {
		const string raw_source = "+(5 + 5)";

		vector<Token> expected_tokens = {
			Token { WTokenType::UNARY_PLUS, "+" },
			Token { WTokenType::OPEN_PARENTHESIS, "(" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::PLUS, "+" },
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::CLOSE_PARENTHESIS, ")" },
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}

SCENARIO("Scanning Binary Expressions", "[Lexer]") {
	GIVEN("Addition : 5 + 1") {
		const string raw_source = "5 + 1";

		vector<Token> expected_tokens = {
			Token { WTokenType::NumberLiteral, "5" },
			Token { WTokenType::PLUS, "+" },
			Token { WTokenType::NumberLiteral, "1" }
		};

		LEXER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}