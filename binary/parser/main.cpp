/*
 * convert_to_csv (in-file) (data-file) (header-file)
 * 
 * Convert the input data into a CSV file with the following format:
 * 
 * ax | ay | az | gx | gy | gz | date_time | temp | pressure
 * ---|----|----|----|----|----|-----------|------|----------
 *    |    |    |    |    |    | 1/1/1111  | 24   | 0
 *  0 |  0 |  0 |  0 |  0 |  0 |           |      |
 * ...
 * 
 * The first line of the file will only contain date and time.
 * The file then alternates between providing accelerometer/gyroscope
 * data, leaving the other lines blank, and providing the long-term
 * data, leaving the accelerometer and gyroscope blank.
 * 
 * It also has a separate header file, technically in a CSV-esque
 * format, that provides data from the tag:
 * name | xbias | ybias | zbias
 * -----|-------|-------|-------
 *      |       |       |
 * 
 */
#include "parser.hpp"
#include <iostream>

constexpr char accel_magic[4] = {'A','C','C','L'};
constexpr char  gyro_magic[4] = {'G','Y','R','O'};
constexpr char  long_magic[4] = {'L','O','N','G'};

void write_header(std::ofstream &file, header_data &header) {
	file << "name,xbias,ybias,zbias" << std::endl;
	file << header.name << ','
		 << header.gyro_bias_x << ','
		 << header.gyro_bias_y << ','
		 << header.gyro_bias_z << std::endl;
}

void write_short_term(std::ofstream &file,
					  std::vector<accel_data> accel,
					  std::vector<gyro_data> gyro) {
	
}

void write_long_term(std::ofstream &file, long_term_data &data) {
	
}

/*
 * Read the next four bytes and see if they match the
 * expected "magic word/number"
 */
bool magic_equals(char expected[4], std::ifstream &file) {
	char real[4];
	file.read(real, 4);
	for (int i = 0; i < 4; i++) {
		if (expected[i] != real[i]) {
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	using namespace std;
	if (argc < 4) {
		cout << "Usage: (infile) (outfile) (header-file)" << endl;
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
	float accel_scale = header->accel_scale;
	float gyro_scale = header->gyro_scale;
	write_header(header_file, *header);
	
	// Start going through sections
	while (!in_file.eof()) {
		char magic[5];
		magic[4] = '\0';
		// Accel
		in_file.read(magic,4);
		std::cout << "Just read: " << magic << std::endl;
		auto accel = parse_accel(in_file, accel_scale, accel_size);
		// Gyro
		in_file.read(magic,4);
		std::cout << "Just read: " << magic << std::endl;
		auto gyro = parse_gyro(in_file, gyro_scale, gyro_size);
		write_short_term(data_file, accel, gyro);
		// Long-term
		in_file.read(magic,4);
		std::cout << "Just read: " << magic << std::endl;
		auto long_term = parse_long_term(in_file);
		write_long_term(data_file, *long_term);
	}
	
	return EXIT_SUCCESS;
}
