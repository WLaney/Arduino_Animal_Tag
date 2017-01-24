#include "output.hpp"
#include "debug.h"
#include "accel.hpp"
#include "gyro.hpp"
#include "rtc.hpp"
#include "temp.hpp"
#include "pressure.hpp"
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

constexpr byte cs_sd = 10;
constexpr byte long_term_write_max = 3;
constexpr char *file_name = "DATA.SRK";

// Header data is packed into this when
// written to the SD card
struct header_data
{
  char name[4];            // Name of device [4 chars]
  byte orient;             // Orientation
  float gx, gy, gz;        // Gyroscope Bias
  unsigned short aws, gws; // Accelerometer and Gyroscope write size
  unsigned short period;
  float as, gs;            // Accelerometer and gyroscope scale
};

bool output_setup() {
  // Setup SD card
  SPI.setDataMode(SPI_MODE0); // unnecessary?
  pinMode(cs_sd, OUTPUT);
  return SD.begin(cs_sd);
}

bool output_write_header() {
  // Get header information
  header_data header;
  // Name of device
  for (byte i=0; i<4; i++) {
    header.name[i] = EEPROM.read(i);
  }
  // Orientation
  header.orient = EEPROM.read(4);
  EEPROM.get(5, header.gx);
  EEPROM.get(9, header.gy);
  EEPROM.get(13, header.gz);
  // Accelerometer, Gyroscope Write Size
  header.aws = accel_write_size();
  header.gws = gyro_write_size();
  // Write period (long_term_write_max)
  header.period = long_term_write_max;
  // Scaling
  header.as = accel_scale();
  header.gs = gyro_scale();
  // Read time before opening card to save power
  rtc_update();
  
  // Write header (and time) to SD
  File sd = SD.open(file_name, FILE_WRITE);
  delay(100);
  if (sd) {
    DBGSTR("Header bytes written: ");
    DBGLN(sd.write((byte *) &header, sizeof(header_data)));
    rtc_write(sd);
    sd.close();
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
    file.print("GYRO");
    gyro_write(file);
    // Long-term writes
    if (long_data) {
      file.print("LONG");
      rtc_write(file);
      temp_write(file);
    }
    file.close();
    DBGSTR("sd written written\n");
  } else {
    DBGSTR("sd could not be opened\n");
  }
  DBG(millis() - time);
  DBGSTR(" ms to write\n");
}
