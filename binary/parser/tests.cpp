/*
 * Test to see that the parser can accurately decode information,
 * both from individual data sections and full files.
 */
#include "parser.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>

/*
 * Quickly get the size of a file.
 * This admittedly doesn't use stat, and can be innacurate
 * in some edge-case circumstances.
 */
inline size_t get_filesize(const char *fname) {
    std::ifstream fs(fname, std::ios::ate | std::ios::binary);
    return fs.tellg();
}

void test_gyro() {
    const char *name = "data/gyro.dat";
    std::ifstream fs(name, std::ios::binary);
    std::vector<gyro_data> data = parse_gyro(fs, 250.0, get_filesize(name));
    if (data.size() > 0) {
        for (auto &read : data) {
          std::cout << read.x << '\t' << read.y << '\t' << read.z << std::endl;
        }
    } else {
        std::cout << "Could not parse gyroscope file: returned null" << std::endl;
    }
}

void test_accel() {
    const char *name = "data/accel.dat";
    std::ifstream fs(name, std::ios::binary);
    std::vector<accel_data> data = parse_accel(fs, 8.0, get_filesize(name));
    if (data.size() > 0) {
        for (auto &read : data) {
          std::cout << read.x << '\t' << read.y << '\t' << read.z << std::endl;
        }
    } else {
        std::cout << "Could not parse accelerometer file: returned null" << std::endl;
    }
}

void test_header() {
    std::cout << "Haven't implemented test_header yet" << std::endl;
}

void test_long_term() {
    std::cout << "Haven't implemented test_long_term yet" << std::endl;
}

int main(void) {
    using namespace std;
    
    cout << "Gyroscope" << endl;
    test_gyro();
    cout << "Accelerometer" << endl;
    test_accel();
    cout << "Header" << endl;
    test_header();
    cout << "Long-Term" << endl;
    test_long_term();
    
    return EXIT_SUCCESS;
}
