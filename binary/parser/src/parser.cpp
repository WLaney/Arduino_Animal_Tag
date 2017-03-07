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
	return s * float{scale / (1 << 13)};
}


/*
 * Convert a raw gyroscope value into a float.
 */
inline float gyro_s2f(short s, float scale) {
	// Should only have these scales
    int scs = (int) scale;
	if (scs != 2000 && scs != 1000 && scs != 500  && scs != 250) {
		std::cout << "ERROR: Invalid gyroscope scale " << scale << std::endl;
		exit(EXIT_FAILURE);
	}
	return ((float) s) * scale / float{1 << 15};
}

/*
 * Read from a file and convert into a certain type
 */
template<typename T>
inline void read_into(std::ifstream &f, T &v) {
	 f.read(reinterpret_cast<char *>(&v), sizeof(T));
}

/*
 * Printing a time struct as Y-M-D h:m:s
 */
std::ostream& operator<<(std::ostream &s, const ts &t) {
	s << (int) t.year << '-' << (int) t.mon << '-' << (int) t.mday << '\t'
	  << (int) t.hour << ':' << (int) t.min << ':' << (int) t.sec;
	return s;
}

/*
 * Due to word alignment, the Arduino and Windows time structs aren't
 * quite identical. As a result, we have to do this...
 */
void read_time(std::ifstream &file, ts &time) {
	 read_into(file, time.sec);
	 read_into(file, time.min);
	 read_into(file, time.hour);
	 read_into(file, time.mday);
	 read_into(file, time.mon);
	 read_into(file, time.year);
	 read_into(file, time.wday);
	 read_into(file, time.yday);
	 read_into(file, time.isdst);
	 read_into(file, time.year_s);
}

/*
 * Parse in_file's header and return the relevant data.
 * 
 * The section must immediately follow the file pointer and be
 * correctly formatted.
 */
std::unique_ptr<header_data>
parse_header(std::ifstream &in_file) {
	std::unique_ptr<header_data> data(new header_data);
	// name is stored with no null char, so we insert it here
	in_file.read(reinterpret_cast<char *>(&(data->name)), 4);
	data->name[4] = '\0';
	read_into(in_file, data->orient);
	read_into(in_file, data->gyro_bias_x);
	read_into(in_file, data->gyro_bias_y);
	read_into(in_file, data->gyro_bias_z);
	read_into(in_file, data->accel_section_size);
	read_into(in_file, data->gyro_section_size);
	read_into(in_file, data->long_term_period);
	read_into(in_file, data->accel_scale);
	read_into(in_file, data->gyro_scale);
	read_time(in_file, data->time);
	return data;
}

/*
 * Parse size bytes' worth of accelerometer reads into floats.
 * 
 * Accelerometer data must immediately follow the file pointer and be
 * correctly formatted.
 */
std::vector<accel_data>
parse_accel(std::ifstream &in_file, float scale, uint16_t size) {
	int reads = size / sizeof(raw_accel_data);
	std::vector<accel_data> out;
	for (int i = 0; i < reads; i++) {
		raw_accel_data raw;
		accel_data prc;
		read_into<raw_accel_data>(in_file, raw);
		// Unpack data into little-endian shorts
		raw.x = (((raw.x & 0xFF00) >> 6) & 0x00FF) | (raw.x & 0x00FF) << 8; raw.x >>= 2;
		raw.y = (((raw.y & 0xFF00) >> 6) & 0x00FF) | (raw.y & 0x00FF) << 8; raw.y >>= 2;
		raw.z = (((raw.z & 0xFF00) >> 6) & 0x00FF) | (raw.z & 0x00FF) << 8; raw.z >>= 2;
		// Convert to floats
		prc.x = accel_s2f(raw.x, scale);
		prc.y = accel_s2f(raw.y, scale);
		prc.z = accel_s2f(raw.z, scale);
		// Push to out
		out.push_back(prc);
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
parse_gyro(std::ifstream &in_file, float scale, uint16_t size, bool orient) {
	int reads = size / sizeof(raw_gyro_data);
	std::vector<gyro_data> out;
	for (int i = 0; i < reads; i++) {
		gyro_data data;
		raw_gyro_data raw;
		read_into(in_file, raw);
		data.x = gyro_s2f(raw.x, scale);
		data.y = gyro_s2f(raw.y, scale);
		data.z = gyro_s2f(raw.z, scale);
		if (orient) {
			std::swap(data.x, data.y);
		}
		out.push_back(data);
	}
	return out;
}

/*
 * Parse and return long term data.
 * 
 * The data must immediately follow the file pointer and be
 * correctly formatted.
 */
std::unique_ptr<long_term_data>
parse_long_term(std::ifstream &in_file) {
	std::unique_ptr<long_term_data> data(new long_term_data);
	read_time(in_file, data->time);
	read_into(in_file, data->celsius);
	return data;
}
