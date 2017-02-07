#include "parser.hpp"
#include <iostream>
#include <string>
#include <utility>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE ( "Individual sections are parsed" ) {
	SECTION("Time") {
		// expected objects don't resemble data, and are designed
		// to be easy to make and check against
		const char *name = "test-data/ts.test";
		std::ifstream file(name, std::ios::binary);
		ts actual;
		read_time(file, actual);
		REQUIRE(actual.sec    == 0);
		REQUIRE(actual.min    == 1);
		REQUIRE(actual.hour   == 2);
		REQUIRE(actual.mday   == 3);
		REQUIRE(actual.mon    == 4);
		REQUIRE(actual.year   == 5);
		REQUIRE(actual.wday   == 6);
		REQUIRE(actual.yday   == 7);
		REQUIRE(actual.isdst   == 8);
		REQUIRE(actual.year_s == 9);
	}
}
