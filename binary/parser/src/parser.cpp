/*
 * Functions for parsing each data section of our file format.
 * The functions are currently not protected against corrupted
 * or incorrect data, which is... possibly bad.
 */
#include <iostream>
#include <iomanip>
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
 * Printing a time struct as YYYY-MM-DD_hh-mm-ss for the filename
 */
std::ostream& operator<<(std::ostream &s, const ts &t) {
	s.fill('0');
	s << (int) t.year << '-'
	  << std::setw(2) << (int) t.mon << '-'
	  << std::setw(2) << (int) t.mday << '_'
	  << std::setw(2) << (int) t.hour << '-'
	  << std::setw(2) << (int) t.min << '-'
	  << std::setw(2) << (int) t.sec;
	return s;
}

/*
 * Printing a time struct as Y-M-D h:m:s for times in the file
 */
void print_time_for_file(std::ostream &s, const ts &t) {
	s << (int) t.year << '-'
	  << (int) t.mon << '-'
	  << (int) t.mday << '\t'
	  << (int) t.hour << ':'
	  << (int) t.min << ':'
	  << (int) t.sec;
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
	/*
	 * We have to hack things a little here.
	 * Old versions of the parser didn't include the
	 * next three fields and instead went straight to
	 * the ACCL block.
	 *
	 * The next field, sample_rate, is an enum, so this
	 * code breaks only if sample_rate has value equal to
	 * 65.
	 */
	if (in_file.peek() == 'A') {
		data->sample_rate = 255;
		data->hq_accel = 255;
		data->alarm_delay = -1;
		data->version = TAG_VERSION::OLD;
	} else {
		read_into(in_file, data->sample_rate);
		read_into(in_file, data->hq_accel);
		read_into(in_file, data->alarm_delay);
		data->version = TAG_VERSION::NEW_22;
	}

	return data;
}

short parse_accel_axis(short x) {
	byte *byte_ptr = (byte *) &x;
	byte b0 = byte_ptr[0];
	byte b1 = byte_ptr[1];

	short out;
	out = b0;
	out <<= 8;
	out |= b1;
	out >>= 2;
	return out;
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
		raw.x = parse_accel_axis(raw.x);
		raw.y = parse_accel_axis(raw.y);
		raw.z = parse_accel_axis(raw.z);
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
