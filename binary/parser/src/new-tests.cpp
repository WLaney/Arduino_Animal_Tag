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
		REQUIRE(actual->sample_rate == 2);
		REQUIRE(actual->hq_accel    == 1);
		REQUIRE(actual->alarm_delay == 0x12345678);
	}
	
	SECTION("Long-Term") {
		auto file = get_test_data("long-term.test");
		auto actual = parse_long_term(*file);
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
		REQUIRE(actual->celsius == 0.5f);
		//REQUIRE(actual->pressure == 0.0f);
	}

	SECTION("Gyroscope") {
		auto file = get_test_data("gyro.test");
		// 3 reads, 3 samples per read, 2 bytes per sample
		auto actual = parse_gyro(*file, 2000.0, 3 * 3 * 2, 0);
		CHECK(actual[0].x ==  Approx(1500.0));
		CHECK(actual[0].y ==  Approx(1000.0));
		CHECK(actual[0].z ==  Approx( 500.0));
		CHECK(actual[1].x ==  Approx( 250.0));
		CHECK(actual[1].y ==  Approx( 125.0));
		CHECK(actual[1].z ==  Approx(  62.5));
		CHECK(actual[2].x ==  Approx( 31.25));
		CHECK(actual[2].y ==  Approx(15.625));
		CHECK(actual[2].z ==  Approx(7.8125));
	}

	SECTION("Accel") {
		auto file = get_test_data("accel.test");
		// two packs of six reads each, two bytes per read
		auto actual = parse_accel(*file, float{1 << 11}, 2 * 6 * 2);
		REQUIRE(actual.size() == 4);
		CHECK(actual[0].x == Approx(2000.0));
		CHECK(actual[0].y == Approx(2001.0));
		CHECK(actual[0].z == Approx(2002.0));
		CHECK(actual[1].x == Approx(2003.0));
		CHECK(actual[1].y == Approx(2004.0));
		CHECK(actual[1].z == Approx(2005.0));
		CHECK(actual[2].x == Approx(2006.0));
		CHECK(actual[2].y == Approx(2007.0));
		CHECK(actual[2].z == Approx(2008.0));
		CHECK(actual[3].x == Approx(2009.0));
		CHECK(actual[3].y == Approx(2010.0));
		CHECK(actual[3].z == Approx(2011.0));
	}
	


}
