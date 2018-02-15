/*
 * Contains utilities for parsing each section.
 */
#ifndef __CONVERT_TO_CSV_HPP__
#define __CONVERT_TO_CSV_HPP__
#include <fstream>
#include <memory>
#include <vector>
#include <stdint.h>

typedef uint8_t byte;

enum class TAG_VERSION {
	/*
	 * A new version of the parser for 2.1 and
	 * 2.2 tags with some extra metadata.
	 */
	NEW_22,
	/*
	 * Older versions of the parser, though still
	 * only for 2.1 and 2.2 tags.
	 */
	OLD
};

/*
 * Data representing time. Taken directly from ds3234.h, it has a lot of
 * irrelevant or unset data. Everything from sec to year should be accurate
 */
struct ts {
	byte sec;
	byte min;
	byte hour;
	byte mday;
	byte mon;
	int16_t year;
	byte wday;
	byte yday;
	byte isdst;
	byte year_s;
};
std::ostream& operator<<(std::ostream&, const ts&);
void print_time_for_file(std::ostream &s, const ts &t);

/*
 * Data from the start of the file. Some of this is thrown into the header.csv;
 * the rest is used to parse the rest of the file
 */
struct header_data {
	char name[5];
	byte orient;
	float gyro_bias_x;
	float gyro_bias_y;
	float gyro_bias_z;
	uint16_t accel_section_size;
	uint16_t gyro_section_size;
	uint16_t long_term_period;
	float accel_scale;
	float gyro_scale;
	ts time;
	byte sample_rate;
	byte hq_accel;
	int32_t alarm_delay;
	TAG_VERSION version;
};

/*
 * A raw accelerometer read from the Accel_1Q library. Stores signed
 * big-endian 14-bit integers, arranged such that the last two bits of
 * the LSB are padding
 */
struct raw_accel_data {
	short x, y, z;
};

/*
 * A raw gyro read.
 */
struct raw_gyro_data {
	short x, y, z;
};

/*
 * Processed data from the accelerometer and gyroscope.
 */
struct accel_data {
	float x, y, z;
};

struct gyro_data {
	float x, y, z;
};

/*
 * Data from the RTC, temperature, and pressure sensors.
 */
struct long_term_data {
	ts time;
	float celsius;
	float millibars;
};

void read_time(std::ifstream&, ts&);

std::unique_ptr<header_data> parse_header(std::ifstream &in_file);
std::vector<accel_data> parse_accel(std::ifstream &in_file, float scale, uint16_t size);
std::vector<gyro_data> parse_gyro(std::ifstream &in_file, float scale, uint16_t size, bool orient);
std::unique_ptr<long_term_data> parse_long_term(std::ifstream &in_file);

#endif /*__CONVERT_TO_CSV_HPP__*/
