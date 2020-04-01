#pragma once
#include "pch.h"
#include "catch.hpp"
#include "test_utils.h"

#include "Parser.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
using Catch::Matchers::Equals;

SCENARIO("Parsing Unary Expressions", "[Parser]") {
	GIVEN("Bang Operator : !true") {
		vector<Token> tokens = {
			CREATE_BANG,
			CREATE_TRUE
		};

		PARSER_ACT;
		REQUIRE_THAT(actual_tokens, Equals(expected_tokens));
	}
}