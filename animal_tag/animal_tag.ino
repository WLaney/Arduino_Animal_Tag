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
  
  // Collect and write tag's configuration data to SD
  char name[5]; // 4-char name
  byte orient;  // incorrect orientation? (swap gyroscope x and y)
  for (byte i=0; i<4; i++) {
    name[i] = EEPROM.read(i);
  }
  name[4] = '\0';
  orient = EEPROM.read(4);
  // Gyroscope bias
  float gx, gy, gz;
  EEPROM.get(5, gx);
  EEPROM.get(9, gy);
  EEPROM.get(13, gz);
  // Time
  rtc_mode();
  rtc_update();
  
  sd_mode();
  File f = SD.open("data.txt", FILE_WRITE);
  // Wait for SD startup
  delay(100);
  if (f) {
    // Name, Orient
    if (name[0] != 255) {
      f.print(name);
      f.print('\t');
      f.println(orient);
      f.print(gx); f.print('\t');
      f.print(gy); f.print('\t');
      f.println(gz);
    }
    // Time
    f.print(F("\t\t\t\t\t\t"));
    rtc_write(f);
    f.write('\n');
    f.close();
    DBGSTR("SD written\n");
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
  static int write_num = 0;
  const int write_max = 3;

  // Read long-term from sensors
  if (++write_num == write_max) {
    write_num = 0;
    rtc_mode();
    rtc_update();
    temp_update();
  }

  DBGSTR("Writing to SD...\n");
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    // Write accelerometer, then gyro, data
    file.println("ACCEL");
    accel_write(file);
    file.println("GYRO");
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

