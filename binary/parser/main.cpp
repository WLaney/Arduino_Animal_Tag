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
 * It also includes a header file containing name and a couple of
 * other things, though it can still be imported with MATLAB's
 * readtable() format.
 */
#include "parser.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
	using namespace std;
	if (argc < 4) {
		cout << "Usage: (infile) (outfile) (header-file)" << endl;
		return EXIT_FAILURE;
	}
	ifstream in_file(argv[1], ios::binary);
	ofstream data_file(argv[2]);
	ofstream header_file(argv[3]);
	 
	/*
	 * until we run out of file:
	 *   N times:
	 *     read an accelerometer and gyroscope section and put them into the CSV
	 *   read a long-term data section and put it into the CSV
	 */
	
	return EXIT_SUCCESS;
}
