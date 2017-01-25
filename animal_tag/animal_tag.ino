#include <SPI.h>          // serial, sd card
#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping
#include <EEPROM.h>       // Configuration
#include "debug.h"
#include "output.hpp"
#include "accel.hpp"
#include "gyro.hpp"
#include "rtc.hpp"
#include "temp.hpp"
#include "pressure.hpp"

constexpr byte long_term_write_max = 3;
byte long_term_write_num = 0;

void setup()
{
  // Debug commands won't show up if they're
  // turned off in debug.h
  DBEGIN();
  accel_setup();
  gyro_setup();
  temp_setup();
  rtc_setup();
  if (!output_setup()) {
    DBGSTR("SD card not inserted. Insert it and restart.");
    while (true)
      ;
  }
  if (!output_write_header()) {
    DBGSTR("ERROR: COULD NOT WRITE HEADER\n");
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
  // Read long-term from sensors
  if (++long_term_write_num == long_term_write_max) {
    long_term_write_num = 0;
    output_write_data(true);
  } else {
    output_write_data(false);
  }
}

// A special version of delay() with
// less accuracy, lower power use
inline void n_delay(long duration) {
  DEND();
  Narcoleptic.delay(duration);
  DBEGIN();
}

