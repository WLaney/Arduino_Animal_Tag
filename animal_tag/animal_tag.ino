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
  if (output_setup() && output_write_header()) {
    DBGSTR("Setup successful\n");
  } else {
    DBGSTR("SD card failed to initialize. Insert it and restart\n");
    // SPI uses pin 13 for something, so we turn it off
    SPI.end();
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH); n_delay(500);
      digitalWrite(LED_BUILTIN, LOW);  n_delay(500);
    }
  }
}

void loop() {
  // We hold the first N gyroscope values in the software buffer; the rest
  // are kept in the hardware buffer
  for (byte i=0; i<gyro_size(); i++) {
    accel_read();
    n_delay(80);
  }
  if (gyro_is_active()) {
    gyro_read_all();
  }
  while (!accel_full()) {
    accel_read();
    n_delay(80);
  }
  flush_and_write();
  accel_reset();
  gyro_reset();
  // Right now, the gyroscope is on for every other read
  // Functionality is subject to change
  if (gyro_is_active()) {
    gyro_set_active(false);
  } else {
    gyro_set_active(true);
  }
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

