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
	return (float) s / (float)(1<<11) * scale;
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
	float fs = ((float) s) / 32678.0;
	return fs * scale;
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
		accel_data data1, data2;
		raw_accel_data raw;
		read_into<raw_accel_data>(in_file, raw);
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
parse_gyro(std::ifstream &in_file, float scale, uint16_t size, bool orient) {
	int reads = size / sizeof(raw_gyro_data);
	std::vector<gyro_data> out;
	for (int i = 0; i < reads; i++) {
		gyro_data data;
		raw_gyro_data raw;
		read_into(in_file, raw);
		data.x = gyro_s2f(raw.x, scale);
		data.y = gyro_s2f(raw.y, scale);
		if (orient) {
			std::swap(data.x, data.y);
		}
		data.z = gyro_s2f(raw.z, scale);
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
