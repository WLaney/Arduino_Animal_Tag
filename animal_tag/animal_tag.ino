#include <Wire.h>
#include <SPI.h>          // serial, sd card
#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping
#include <EEPROM.h>
#include "debug.h"
#include "accel.hpp"
#include "gyro.hpp"
#include "rtc.hpp"
#include "temp.hpp"
#include "pressure.hpp"

#define CS_SD 10

void setup()
{
  DBEGIN();

  accel_setup();
  gyro_setup();
  temp_setup();
  rtc_setup();

  // Setup SD card
  sd_mode();
  pinMode(CS_SD, OUTPUT);
  if (!SD.begin(CS_SD)) {
    DBGSTR("SD card not inserted. Insert it and restart.");
    while (true)
      ;
  }
  // Setup wire
  Wire.begin();
  // Header info and time
  char name[5];
  byte orient;
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
  //confrim that the everything is working and there is serial communication
  DBGSTR("setup done\n");
}

void loop() {
  // We hold the first N gyroscope values in the software buffer; the rest
  // are kept in the hardware buffer
  for (byte i=0; i<gyro_size(); i++) {
    accel_read();
    DEND();
    Narcoleptic.delay(80);
    DBEGIN();
  }
  gyro_read_all();
  while (!accel_full()) {
    accel_read();
    DEND();
    Narcoleptic.delay(80);
    DBEGIN();
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

  //Write to SD
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    // Write buffered data, interleaving writes
    for (byte ai=0, gi=0; ai<accel_size(); ai++) {
      accel_write(file, ai);
      file.write('\t');
      gyro_write(file, gi++);
      if (gi == gyro_size()) {
        gyro_read_all();
        gi = 0;
      }
      file.println();
    }
    // Long-term
    if (write_num == 0) {
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
  accel_reset();
  gyro_reset();
}

inline void sd_mode() {
  SPI.setDataMode(SPI_MODE0);
}

inline void rtc_mode() {
  SPI.setDataMode(SPI_MODE1);
}
