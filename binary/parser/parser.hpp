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

/*
 * The order of this struct matters, since we're importing it wholesale.
 * Don't change it!
 *
 * Data representing time. Culled directly from ds3234.h
 */
struct ts {
	byte sec;
	byte min;
	byte hour;
	byte mday;
	byte mon;
	// single byte of padding
	int16_t year;
	byte wday;
	byte yday;
	byte isdst;
	byte year_s;
};
std::ostream& operator<<(std::ostream&, const ts&);

/*
 * All the data collected from the header. Some of it's relevant to 
 * the header file; the rest is relevant to parsing the rest of the file.
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
};

/*
 * A raw accelerometer read. 3 12-bit integers are packed into 
 * 9 bytes
 */
struct raw_accel_data {
	// We store the full LSBs of each read in here
	struct {
		byte x1, y1, z1;
		byte x2, y2, z2;
	} lsb_bytes;
	// Then we store each "nibble" (the top 4 bits)
	// r1 takes the most significant nibble, and r2 the least
	struct {
		byte x1_x2;
		byte y1_y2;
		byte z1_z2;
	} msb_nibbles;
};

/*
 * A raw gyro read.
 */
struct raw_gyro_data {
	short x, y, z;
};

/*
 * Processed data from the accelerometer an gyroscope.
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

std::unique_ptr<header_data> parse_header(std::ifstream &in_file);
std::vector<accel_data> parse_accel(std::ifstream &in_file, float scale, uint16_t size);
std::vector<gyro_data> parse_gyro(std::ifstream &in_file, float scale, uint16_t size);
std::unique_ptr<long_term_data> parse_long_term(std::ifstream &in_file);

#endif /*__CONVERT_TO_CSV_HPP__*/
