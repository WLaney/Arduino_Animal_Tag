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
  SPI.setDataMode(SPI_MODE0); // unnecessary?
  pinMode(cs_sd, OUTPUT);
  bool sd_active = SD.begin(cs_sd);
  if (!sd_active) {
	  return false;
  }
  set_file_name(file_name);
  DBGSTR("Out file: "); DBGLN(file_name);
  return true;
}

bool output_write_header(header_data &header) {
  rtc_update();
  // Write header (and time) to SD
  File sd = SD.open(file_name, FILE_WRITE);
  delay(100);
  if (sd) {
    // Bleaugh. Sorry.
    byte b = sd.write((byte *) &header, 31);
    rtc_write(sd);
    b += sd.write(42 + ((byte *) &header), 6);
    sd.close();
    DBGSTR("Header bytes written (minus time): ");
    DBGLN(b);
    return true; 
  } else {
    return false;
  }
}

void output_write_data(bool long_data) {

  if (long_data) {
    rtc_update();
    temp_update();
  }
  DBGSTR("Writing to SD...\n");
  long time = millis();
  File file = SD.open(file_name, FILE_WRITE);
  if (file) {
    // Accelerometer/gyro writes
    file.print("ACCL");
    accel_write(file);
	if (gyro_is_active()) {
    	file.print("GYRO");
    	gyro_write(file, accel_downscaled());
	} else {
		file.print("GSKP");
	}
    // Long-term writes
    if (long_data) {
      file.print("LONG");
      rtc_write(file);
      temp_write(file);
      DBGSTR("Long.write\n");
    }
    file.close();
  } else {
    DBGSTR("ERROR: couldn't write to SD\n");
  }
  DBG(millis() - time);
  DBGSTR(" ms to write\n");
}

/*
 * Tries to set c to an unused filename of the format DATA-XXX.SRK,
 * 
 */
static void set_file_name(char *c) {
  c[0]='D'; c[1]='A'; c[2]='T'; c[3]='A';
  // c[5..7] are set below
  c[8]='.'; c[9]='S'; c[10]='R'; c[11]='K'; c[12]='\0';
  
  // Find the lowest number to use (max 255)
  unsigned char n = 0;
  do {
    c[5] = '0' + (n / 100) % 10;
    c[6] = '0' + (n / 10) % 10;
    c[7] = '0' + n % 10;
    if (!SD.exists(c)) {
      break;
    }
    n++;
  } while (n != 0);
  
  if (n == 255) {
    c[5] = 'M';
    c[6] = 'A';
    c[7] = 'X';
  }
}
