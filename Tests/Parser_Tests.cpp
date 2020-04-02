#pragma once
#include "pch.h"
#include "catch.hpp"

#include "test_utils.h"
#include "Parser.h"
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using Catch::Matchers::Equals;
//
//SCENARIO("Parsing Unary Expressions", "[Parser]") {
//	GIVEN("Bang Operator : !true") {
//		vector<Token> tokens = {
//			CREATE_BANG,
//			CREATE_TRUE
//		};
//
//		vector<Statement> expected_statements = {
//		};
//
//		PARSER_ACT;
//		REQUIRE_THAT(actual_statements, Equals(expected_statements));
//	}
//}