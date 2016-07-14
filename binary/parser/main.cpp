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
#include <assert.h>

constexpr char *accel_magic = "ACCL";
constexpr char *gyro_magic = "GYRO";
constexpr char *long_term_magic = "LONG";

/*
 * Grab the next four bytes and see if they match a magic word.
 * If not, print an error and exit. (TODO Use exceptions!)
 */
void check_magic_word(std::ifstream &in_file, const char *expected) {
	char actual[5];
	in_file.read(actual, 4);
	actual[4] = '\0';
	if (strncmp(expected, actual, 4) != 0) {
		std::cerr << "ERROR: expected header " << expected << ", got " << actual << std::endl;
		exit(EXIT_FAILURE);
	}
}

void write_header(std::ofstream &hfile, std::ofstream &dfile, header_data &header) {
	hfile << "name,xbias,ybias,zbias" << std::endl;
	hfile << header.name << ','
		  << header.gyro_bias_x << ','
		  << header.gyro_bias_y << ','
		  << header.gyro_bias_z << std::endl;
	
	dfile << ",,,,,," << header.time << ",," << std::endl;
}

void write_short_term(std::ofstream &file,
					  std::vector<accel_data> accel,
					  std::vector<gyro_data> gyro) {
	assert(accel.size() == gyro.size());
	auto ai = accel.begin();
	auto gi = gyro.begin();
	while (ai != accel.end()) {
		file << ai->x << ',' << ai->y << ',' << ai->z << ','
		     << gi->x << ',' << gi->y << ',' << gi->z
			 << ",,," << std::endl;
		ai++, gi++;
	}
}

void write_long_term(std::ofstream &file, long_term_data &data) {
	file << ",,,,,," << data.time << ',' << data.celsius << ",0" << std::endl;
}

int main(int argc, char *argv[]) {
	using namespace std;
	if (argc < 4) {
		cout << "Usage: (infile) (outfile) (header-file)" << endl
		     << "See readme.txt for more information." << endl;
		return EXIT_FAILURE;
	}
	ifstream in_file(argv[1], ios::binary);
	ofstream data_file(argv[2]);
	ofstream header_file(argv[3]);
	
	// Parse header; get relevant data
	unique_ptr<header_data> header = parse_header(in_file);
	bool orient = header->orient;
	size_t accel_size = header->accel_section_size;
	size_t gyro_size = header->gyro_section_size;
	int long_term_period = header->long_term_period;
	float accel_scale = header->accel_scale;
	float gyro_scale = header->gyro_scale;
	write_header(header_file, data_file, *header);
	
	// Start going through sections
	data_file << "ax,ay,az,gx,gy,gz,date_time,temp,pressure" << endl;
	int long_term_index = 0;
	while (!in_file.eof()) {
		if (long_term_index++ < long_term_period) {
			check_magic_word(in_file, accel_magic);
			auto accel_data = parse_accel(in_file, accel_scale, accel_size);
			check_magic_word(in_file, gyro_magic);
			auto gyro_data = parse_gyro(in_file, gyro_scale, gyro_size, orient);
			write_short_term(data_file, accel_data, gyro_data);
		} else {
			check_magic_word(in_file, long_term_magic);
			write_long_term(data_file, *parse_long_term(in_file));
			long_term_index = 0;
		}
	}
	
	return EXIT_SUCCESS;
}
