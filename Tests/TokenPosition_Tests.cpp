#pragma once
#include "pch.h"
#include "catch.hpp"
#include "TokenPosition.h"

SCENARIO("Sanity Checking the TokenPosition class to manipulate an index", "[TokenPosition]") {
	GIVEN("A fresh TokenPosition object") {
		TokenPosition position;

		REQUIRE(position.get_line_num() == 1);
		REQUIRE(position.get_column_num() == 1);

		WHEN("line number of incremented") {
			position.increment_line_number();

			THEN("line number must increase to 2") {
				REQUIRE(position.get_line_num() == 2);
			}
		}

		WHEN("column number of incremented") {
			position.increment_column_number();

			THEN("column number must increase by 1") {
				REQUIRE(position.get_column_num() == 2);
			}
		}

		WHEN("column number is reset") {
			position.reset_column_number();

			THEN("column number must be set to 1") {
				REQUIRE(position.get_column_num() == 1);
			}
		}
	}
}