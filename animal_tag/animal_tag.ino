#include <SPI.h>          // serial, sd card
#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping
#include <EEPROM.h>       // Configuration
#include "debug.h"
#include "accel.hpp"
#include "gyro.hpp"
#include "rtc.hpp"
#include "temp.hpp"
#include "pressure.hpp"

constexpr byte cs_sd = 10;
constexpr short long_term_write_max = 3;

// Header data is packed into this when
// written to the SD card
struct header_data {
  char name[4];            // Name of device [4 chars]
  byte orient;             // Orientation
  float gx, gy, gz;        // Gyroscope Bias
  unsigned short aws, gws; // Accelerometer and Gyroscope write size
  unsigned short period;
  float as, gs;            // Accelerometer and gyroscope scale
};

void setup()
{
  // Debug commands won't show up if they're
  // turned off in debug.h
  DBEGIN();
  
  // Setup sensors
  accel_setup();
  gyro_setup();
  temp_setup();
  rtc_setup();
  
  // Setup SD card
  sd_mode();
  pinMode(cs_sd, OUTPUT);
  if (!SD.begin(cs_sd)) {
    DBGSTR("SD card not inserted. Insert it and restart.");
    while (true)
      ;
  }

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
  // RTC Update (written separately)
  rtc_mode();
  rtc_update();

  // Write header (and time) to SD
  sd_mode();
  File sd = SD.open("DATA.TXT", FILE_WRITE);
  delay(100);
  if (sd) {
    DBGSTR("Header bytes written: ");
    DBGLN(sd.write((byte *) &header, sizeof(header_data)));
    rtc_write(sd);
    sd.close();
    
    DBGLN("Header written");
  } else {
    DBGSTR("ERROR: Header not written!");
  }
  
  DBGSTR("setup done\n");
}

void loop() {
  // We hold the first N gyroscope values in the software buffer; the rest
  // are kept in the hardware buffer
  for (byte i=0; i<gyro_size(); i++) {
    accel_read();
    n_delay(80);
  }
  gyro_read_all();
  while (!accel_full()) {
    accel_read();
    n_delay(80);
  }
  flush_and_write();
}

// Flush the buffer to the SD card, writing temperature and time
// data as you do.
void flush_and_write()
{
  static short write_num = 0;
  // Read long-term from sensors
  if (++write_num == long_term_write_max) {
    write_num = 0;
    rtc_mode();
    rtc_update();
    temp_update();
  }

  DBGSTR("Writing to SD...\n");
  long time = millis();
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    // Write accelerometer, then gyro, data
    file.print("ACCEL");
    accel_write(file);
    file.print("GYRO");
    gyro_write(file);
    // Long-term writes
    if (write_num == 0) {
      file.println("LONG");
      file.print(F("\t\t\t\t\t\t"));
      rtc_write(file);
      file.print('\t');
      temp_write(file);
      file.print("\n");
    }
    file.close();
    DBGSTR("sd data written\n");
  } else {
    DBGSTR("sd could not be opened\n");
  }
  DBG(millis() - time);
  DBGSTR(" ms to write\n");
  accel_reset();
  gyro_reset();
}

// Prep SPI to write to the SD card
inline void sd_mode() {
  SPI.setDataMode(SPI_MODE0);
}

// Prep to work with RTC
inline void rtc_mode() {
  SPI.setDataMode(SPI_MODE1);
}

// A special version of delay() with
// less accuracy, lower power use
inline void n_delay(long duration) {
  DEND();
  Narcoleptic.delay(duration);
  DBEGIN();
}

