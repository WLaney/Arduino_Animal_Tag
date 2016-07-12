/*
 * Functions for parsing each data section of our file format.
 * The functions are currently not protected against corrupted
 * or incorrect data, which is... possibly bad.
 */
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include "parser.hpp"

/*
 * Convert a raw accelerometer read into a float.
 */
inline float accel_s2f(short s, float scale) {
	return (float) s;
}

/*
 * Convert a raw gyroscope value into a float.
 */
inline float gyro_s2f(short s, float scale) {
	return (float) s;
}

/*
 * Parse in_file's header and return the relevant data.
 * 
 * The section must immediately follow the file pointer and be
 * correctly formatted.
 */
std::unique_ptr<header_data>
parse_header(std::ifstream &in_file) {
	return NULL;
}

/*
 * Parse size bytes' worth of accelerometer reads into floats.
 * 
 * Accelerometer data must immediately follow the file pointer and be
 * correctly formatted.
 */
std::vector<accel_data>
parse_accel(std::ifstream &in_file, float scale, uint16_t size) {
	// TODO [check against size]
	int reads = size / sizeof(raw_accel_data);
	std::vector<accel_data> out;
	out.reserve(reads * 2);
	for (int i = 0; i < reads; i++) {
		accel_data data1, data2;
		raw_accel_data raw;
		in_file.read(reinterpret_cast<char*>(&raw), sizeof(raw_accel_data));
		// First, unpack the raw data into shorts
		short x1, y1, z1, x2, y2, z2;
		x1 = (((int8_t) raw.msb_nibbles.x1_x2) >> 4) << 8 | raw.lsb_bytes.x1;
		y1 = (((int8_t) raw.msb_nibbles.y1_y2) >> 4) << 8 | raw.lsb_bytes.y1;
		z1 = (((int8_t) raw.msb_nibbles.z1_z2) >> 4) << 8 | raw.lsb_bytes.z1;
		
		x2 = (int16_t) (raw.msb_nibbles.x1_x2 << 12) >> 4 | raw.lsb_bytes.x2;
		y2 = (int16_t) (raw.msb_nibbles.y1_y2 << 12) >> 4 | raw.lsb_bytes.y2;
		z2 = (int16_t) (raw.msb_nibbles.z1_z2 << 12) >> 4 | raw.lsb_bytes.z2;
		// Convert to floats
		data1.x = accel_s2f(x1, scale);
		data1.y = accel_s2f(y1, scale);
		data1.z = accel_s2f(z1, scale);
		data2.x = accel_s2f(x2, scale);
		data2.y = accel_s2f(y2, scale);
		data2.z = accel_s2f(z2, scale);
		// Push to out
		out.push_back(data1);
		out.push_back(data2);
	}
	return out;
}

/*
 * Parse and return the next size bytes' worth of gyroscope reads.
 * 
 * Gyro data must immediately follow the file pointer and be
 * correctly formatted.
 */
std::vector<gyro_data>
parse_gyro(std::ifstream &in_file, float scale, uint16_t size) {
	// TODO [check against size]
	int reads = size / sizeof(raw_gyro_data);
	std::vector<gyro_data> out;
	out.reserve(reads);
	for (int i = 0; i < reads; i++) {
		gyro_data data;
		raw_gyro_data raw;
		in_file.read(reinterpret_cast<char*>(&raw), sizeof(raw_gyro_data));
		data.x = gyro_s2f(raw.x, scale);
		data.y = gyro_s2f(raw.y, scale);
		data.z = gyro_s2f(raw.z, scale);
		out.push_back(data);
	}
	return out;
}

std::unique_ptr<long_term_data>
parse_long_term(std::ifstream &in_file) {
	return NULL;
}
