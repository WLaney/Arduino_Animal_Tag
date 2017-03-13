#include "output.hpp"
#include "debug.h"
#include "accel.hpp"
#include "gyro.hpp"
#include "rtc.hpp"
#include "temp.hpp"
#include "pressure.hpp"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

constexpr byte cs_sd = 10;
constexpr byte long_term_write_max = 3;
char *file_name = "DATA-XXX.SRK";

static void set_file_name(char *);

bool output_setup() {
  return true;
}

bool output_write_header(header_data &header) {
  DBGSTR("Header (would be) written\n");
  return true;
}

void output_write_data(bool long_data) {
  DBGSTR("Data (would be) written\n");
}

/*
 * Tries to set c to an unused filename of the format DATA-XXX.SRK,
 * 
 */
static void set_file_name(char *c) {
}
