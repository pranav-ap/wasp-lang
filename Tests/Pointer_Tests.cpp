#include "catch.hpp"
#include "Pointer.h"

SCENARIO("Sanity Checking the Pointer class to manipulate an index", "[Pointer]") {
	GIVEN("A fresh pointer") {
		Pointer pointer;

		REQUIRE(pointer.get_index() == 0);

		WHEN("the pointer is advanced") {
			pointer.advance();

			THEN("the index must increase to 1") {
				REQUIRE(pointer.get_index() == 1);
			}

			WHEN("the pointer is retreated") {
				pointer.retreat();

				THEN("the index must go back to 0") {
					REQUIRE(pointer.get_index() == 0);
				}
			}
		}
	}
}