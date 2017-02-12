/*
 * shark-parser (in-file) (data-file) (header-file)
 * 
 * Read readme.txt for more info.
 */
#include "parser.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <string.h>

constexpr char *accel_magic = "ACCL";
constexpr char *gyro_magic = "GYRO";
constexpr char *long_term_magic = "LONG";

const char *magic[] {
	"ACCL",
	"GYRO",
	"GSKP",
	"LONG",
};

enum class MAGIC {
	ACCL = 0,
	GYRO = 1,
	GSKP = 2,
	LONG = 3,
	BROKEN = -1,
};

/*
 * Read the next four bytes and return the magic word
 * that they correspond to. If nothing is found,
 * BROKEN is returned instead.
 */
MAGIC get_next_magic(std::ifstream &in_file) {
	char actual[5];
	in_file.read(actual, 4);
	actual[4] = '\0';
	for (int i = 0; i < sizeof(magic) / sizeof(char *); i++) {
		if (strncmp(actual, magic[i], 4) == 0) {
			return static_cast<MAGIC>(i);
		}
	}
	return MAGIC::BROKEN;
}

/*
 * Grab the next four bytes and see if they match a magic word.
 * If not, print an error and exit.
 *
 * Deprecated in favor of get_next_magic
 */
void check_magic_word(std::ifstream &in_file, const char *expected) {
	// Get next magic
	MAGIC actual = get_next_magic(in_file);
	// Convert enum to string
	const char *actual_s = magic[static_cast<int>(actual)];
	// Compare string
	if (strncmp(actual_s, expected, 4) != 0) {
		std::cerr << "ERROR: expected chunk " << expected << ", got " << actual_s << std::endl;
		exit(1);
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
 * Write just accelerometer data to a file.
 * The skipped gyroscope writes will be represented with an empty cell
 */
void write_short_term_no_gyro(std::ofstream &file,
                            std::vector<accel_data> accel) {
	for (auto a : accel) {
		file << a.x << ',' << a.y << ',' << a.z
		     << ",,,,,," << std::endl;
	}
}

/*
 * Write long-term data to file.
 */
void write_long_term(std::ofstream &file, long_term_data &data) {
	file << ",,,,,," << data.time << ',' << data.celsius << ",0" << std::endl;
}


/*
 * Print a short readme.
 */
void usage(char *fname) {
	std::cout << fname << " (infile) [data-csv] [header-csv]"
			  << std::endl << std::endl
			  << "infile is the raw data to parse. If data-csv and header-csv " 
			  << "are not specified, then their names will be inferred from infile. "
			  << "data-csv contains the raw data, and header-csv contains header data."
			  << std::endl;
}

int main(int argc, char *argv[]) {
	std::string in_fname, data_fname, header_fname;

	// Argument parsing
	if (argc == 2) {
		// Infer output filenames
		in_fname = std::string(argv[1]);
		std::string noext;
		size_t pos;
		// Remove file extension
		noext = in_fname;
		pos = in_fname.rfind('.');
		noext.resize(pos);
		// Add names
		data_fname = noext + "-data.csv";
		header_fname = noext + "-header.csv";
	} else if (argc == 4) {
		// data-csv and header-csv are specified
		in_fname = std::string(argv[1]);
		data_fname = std::string(argv[2]);
		header_fname = std::string(argv[3]);
	} else {
		// incorrect format
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	// Verbosity
	std::clog << "Creating " << data_fname << " and " << header_fname
	          << " from " << in_fname << std::endl;
	// Start trying to open files
	std::ifstream in_file(in_fname, std::ios::binary);
	if (!in_file.is_open()) {
		std::cerr << "ERROR: Could not open " << in_fname << std::endl;
		exit(EXIT_FAILURE);
	}
	std::ofstream data_file(data_fname);
	if (!data_file.is_open()) {
		std::cerr << "ERROR: Could not open " << data_fname << std::endl;
		exit(EXIT_FAILURE);
	}
	std::ofstream header_file(header_fname);
	if (!header_file.is_open()) {
		std::cerr << "ERROR: Could not open " << header_fname << std::endl;
		exit(EXIT_FAILURE);
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
			auto accel_reads = parse_accel(in_file, accel_scale, accel_size);
			// Gyroscope OR Gskip
			MAGIC m = get_next_magic(in_file);
			if (m == MAGIC::GYRO) {
				auto gyro_reads = parse_gyro(in_file, gyro_scale, gyro_size, orient);
				write_short_term(data_file, accel_reads, gyro_reads);
			} else if (m == MAGIC::GSKP) {
				write_short_term_no_gyro(data_file, accel_reads);
			} else {
				std::cerr << "ERROR: expected GYRO or GSKP, got " << magic[static_cast<int>(m)] << std::endl;
			}
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
	
	std::clog << "Successfully wrote to files." << std::endl;
	return EXIT_SUCCESS;
}
