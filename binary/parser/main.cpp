/*
 * shark-parser (in-file) (data-file) (header-file)
 * 
 * Read readme.txt for more info.
 */
#include "parser.hpp"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>

constexpr char *accel_magic = "ACCL";
constexpr char *gyro_magic = "GYRO";
constexpr char *long_term_magic = "LONG";

/*
 * Grab the next four bytes and see if they match a magic word.
 * If not, print an error and exit.
 */
void check_magic_word(std::ifstream &in_file, const char *expected) {
	char actual[5];
	in_file.read(actual, 4);
	actual[4] = '\0';
	if (strncmp(expected, actual, 4) != 0) {
		std::cerr << "ERROR: Expected header " << expected << ", got " << actual << std::endl;
		exit(EXIT_FAILURE);
	}
}

/*
 * Write header data to the header_file and initial time data to data_file..
 */
void write_header(std::ofstream &hfile, std::ofstream &dfile, header_data &header) {
	hfile << header.name << ','
		  << header.gyro_bias_x << ','
		  << header.gyro_bias_y << ','
		  << header.gyro_bias_z << std::endl;
	
	dfile << ",,,,,," << header.time << ",," << std::endl;
}

/*
 * Write accelerometer and gyroscope data to file.
 */
void write_short_term(std::ofstream &file,
					  std::vector<accel_data> accel,
					  std::vector<gyro_data> gyro) {
	auto ai = accel.begin();
	auto gi = gyro.begin();
	while (ai != accel.end()) {
		file << ai->x << ',' << ai->y << ',' << ai->z << ','
		     << gi->x << ',' << gi->y << ',' << gi->z
			 << ",,," << std::endl;
		ai++, gi++;
	}
}

/*
 * Write long-term data to file.
 */
void write_long_term(std::ofstream &file, long_term_data &data) {
	file << ",,,,,," << data.time << ',' << data.celsius << ",0" << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		std::cout << "Usage: (infile) (data-csv) (header-csv)" << std::endl
		          << "See readme.txt for more information." << std::endl;
		return EXIT_FAILURE;
	}
	std::ifstream in_file(argv[1], std::ios::binary);
	if (!in_file.is_open()) {
		std::cerr << "ERROR: Could not open " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}
	std::ofstream data_file(argv[2]);
	if (!data_file.is_open()) {
		std::cerr << "ERROR: Could not open " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}
	std::ofstream header_file(argv[3]);
	if (!header_file.is_open()) {
		std::cerr << "ERROR: Could not open " << argv[3] << std::endl;
		return EXIT_FAILURE;
	}
	
	// Write column names
	data_file << "ax,ay,az,gx,gy,gz,date_time,temp,pressure" << std::endl;
	header_file << "name,xbias,ybias,zbias" << std::endl;

	// Parse header; get relevant data
	std::unique_ptr<header_data> header = parse_header(in_file);
	write_header(header_file, data_file, *header);
	
	size_t accel_size = header->accel_section_size;
	size_t gyro_size = header->gyro_section_size;
	int long_term_period = header->long_term_period;
	
	float accel_scale = header->accel_scale;
	float gyro_scale = header->gyro_scale;
	bool orient = header->orient;
	
	// Go through each section
	int long_term_index = 0;
	while (!in_file.eof()) {
		if (long_term_index++ < long_term_period) {
			// Accelerometer
			check_magic_word(in_file, accel_magic);
			auto accel_data = parse_accel(in_file, accel_scale, accel_size);
			// Gyroscope
			check_magic_word(in_file, gyro_magic);
			auto gyro_data = parse_gyro(in_file, gyro_scale, gyro_size, orient);
			write_short_term(data_file, accel_data, gyro_data);
		} else {
			// Long-term
			check_magic_word(in_file, long_term_magic);
			write_long_term(data_file, *parse_long_term(in_file));
			long_term_index = 0;
		}
		// Check for EOF
		char c;
		in_file.read(&c,1);
		if (in_file.eof())
			break;
		in_file.unget();
	}
	
	return EXIT_SUCCESS;
}
