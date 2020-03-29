#pragma once
#include "pch.h"
#include "catch.hpp"
#include "Token.h"
#include "TokenType.h"

SCENARIO("Sanity Testing the Token Class", "[Token]") {
	GIVEN("A fresh Boolean Token object") {
		Token token(WTokenType::BOOL, "bool", 1, 1);

		REQUIRE(token.get_line_num() == 1);
		REQUIRE(token.get_column_num() == 1);

		REQUIRE(token.get_value() == "bool");
		REQUIRE(token.get_type() == WTokenType::BOOL);
	}
}