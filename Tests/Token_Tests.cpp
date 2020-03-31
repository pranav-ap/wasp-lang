#pragma once
#include "pch.h"
#include "catch.hpp"
#include "Token.h"
#include "TokenType.h"

SCENARIO("Sanity Testing the Token Class", "[Token]") {
	GIVEN("A fresh Boolean Token object") {
		Token token(WTokenType::BOOL, "bool", 1, 1);

		REQUIRE(token.line_num == 1);
		REQUIRE(token.column_num == 1);

		REQUIRE(token.value == "bool");
		REQUIRE(token.type == WTokenType::BOOL);
	}
}