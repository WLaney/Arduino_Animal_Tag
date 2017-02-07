#include "parser.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include <string>
#include <utility>

/*
 * HELPER FUNCTIONS AND VALUES
 */

/*
 * Returng a file handle containing binary data from the test data directory.
 */
std::unique_ptr<std::ifstream> get_test_data(const std::string &filename) {
	std::string path = std::string("test-data/") + filename;
	auto file = std::unique_ptr<std::ifstream>( new std::ifstream(path, std::ios::binary) );
	if (!file->is_open()) {
		FAIL("File could not be opened. This is the fault of the test code");
	}
	return file;
}

/*
 * TEST CASES
 */

TEST_CASE ( "Individual sections are parsed" ) {
	SECTION("Time") {
		// expected objects don't resemble data, and are designed
		// to be easy to make and check against
		auto file = get_test_data("ts.test");
		ts actual;
		read_time(*file, actual);
		REQUIRE(actual.sec    == 0);
		REQUIRE(actual.min    == 1);
		REQUIRE(actual.hour   == 2);
		REQUIRE(actual.mday   == 3);
		REQUIRE(actual.mon    == 4);
		REQUIRE(actual.year   == 5);
		REQUIRE(actual.wday   == 6);
		REQUIRE(actual.yday   == 7);
		REQUIRE(actual.isdst  == 8);
		REQUIRE(actual.year_s == 9);
	}
	
	SECTION("Header") {
		auto file = get_test_data("header.test");
		auto actual = parse_header(*file);
		REQUIRE(std::string(actual->name) == "TEST");
		REQUIRE(actual->orient == 0);
		REQUIRE(actual->gyro_bias_x == 0.5f);
		REQUIRE(actual->gyro_bias_y == 0.25f);
		REQUIRE(actual->gyro_bias_z == 0.125f);
		REQUIRE(actual->accel_section_size == 1);
		REQUIRE(actual->gyro_section_size == 2);
		REQUIRE(actual->long_term_period == 3);
		REQUIRE(actual->accel_scale == 2.0f);
		REQUIRE(actual->gyro_scale == 4.0f);
		REQUIRE(actual->time.sec    == 0);
		REQUIRE(actual->time.min    == 1);
		REQUIRE(actual->time.hour   == 2);
		REQUIRE(actual->time.mday   == 3);
		REQUIRE(actual->time.mon    == 4);
		REQUIRE(actual->time.year   == 5);
		REQUIRE(actual->time.wday   == 6);
		REQUIRE(actual->time.yday   == 7);
		REQUIRE(actual->time.isdst  == 8);
		REQUIRE(actual->time.year_s == 9);
	}
}
