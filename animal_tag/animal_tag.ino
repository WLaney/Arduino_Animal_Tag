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

// Copied from configure.ino
enum SAMPLE_RATE {
    ODR_12_5_HZ,
    ODR_25_HZ,
    ODR_50_HZ
};

constexpr byte long_term_write_max = 3;
byte long_term_write_num = 0;

byte sample_delay;

void setup()
{
  DBEGIN();
  // Delayed startup
  rtc_setup();
  long int delay_time;
  EEPROM.get(21, delay_time);
  if (delay_time > 0) {
    DBGSTR("Sleeping: ");
    DBG(delay_time);
    DBGSTR(" seconds\n");
    DEND();
    rtc_snooze(delay_time);
    DBEGIN();
    delay(250);
  }

  header_data header;
  FXAS2::Range gscale;
  FXAS2::ODR godr;
  Accel::Range ascale;
  Accel::ODR aodr;
  SAMPLE_RATE odr;
  bool downscale;
  
  // Get configuration from EEPROM
  for (byte i=0; i<4; i++) {
    header.name[i] = EEPROM.read(i);
  }
  header.orient = EEPROM.read(4);
  EEPROM.get(5,  header.gx); // gyroscope bias
  EEPROM.get(9,  header.gy);
  EEPROM.get(13, header.gz);
  ascale = static_cast<Accel::Range>(EEPROM.read(17));
  if (ascale < Accel::Range::G2 || ascale > Accel::Range::G8) {
    DBGSTR("ERROR: A.Scale invalid\n");
    ascale = Accel::Range::G4;
  }
  gscale = static_cast<FXAS2::Range>(EEPROM.read(18));
  if (gscale < FXAS2::Range::DPS_2000 || gscale > FXAS2::Range::DPS_250) {
    DBGSTR("ERROR: G.Scale invalid\n");
    gscale = FXAS2::Range::DPS_250;
  }
  odr = static_cast<SAMPLE_RATE>(EEPROM.read(19));
  switch (odr) {
    case ODR_12_5_HZ:
		DBGSTR("12hz\n");
        aodr = Accel::ODR::HZ_12_5;
        godr = FXAS2::ODR::HZ_12_5;
        sample_delay = byte{1000.0 / 12.5};
        downscale = false;
        break;
    case ODR_25_HZ:
		DBGSTR("25Hz\n");
        aodr = Accel::ODR::HZ_50;
        godr = FXAS2::ODR::HZ_25;
        sample_delay = byte{1000.0 / 25.0};
        downscale = true;
        break;
    case ODR_50_HZ:
		DBGSTR("50hz\n");
        aodr = Accel::ODR::HZ_50;
        godr = FXAS2::ODR::HZ_50;
        downscale = false;
        sample_delay = byte{1000.0 / 50.0};
        break;
    default:
        DBGSTR("Bad s.rate - will use 25Hz\n");
        aodr = Accel::ODR::HZ_50;
        godr = FXAS2::ODR::HZ_25;
        downscale = true;
        sample_delay = byte{1000.0 / 25.0};
  }
  
  // Debug commands won't show up if they're
  // turned off in debug.h
  DBEGIN();
  accel_setup(ascale, aodr, downscale);
  gyro_setup(gscale, godr);
  temp_setup();
  
  // Start SD card
  if (output_setup()) {
    DBGSTR("Setup good\n");
  } else {
    DBGSTR("SD card failed; insert it and restart\n");
    // SPI uses pin 13 for something, so we turn it off
    SPI.end();
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH); n_delay(500);
      digitalWrite(LED_BUILTIN, LOW);  n_delay(500);
    }
  }
  
  // Write header
  // Accelerometer, Gyroscope Write Size
  header.aws = accel_write_size();
  // TODO not hack
  if (downscale) {
	header.gws = accel_write_size();
  } else {
    header.gws = gyro_write_size();
  }
  // Write period (long_term_write_max)
  header.period = long_term_write_max;
  // Scaling
  header.as = accel_scale();
  header.gs = gyro_scale();
  output_write_header(header);
}

void loop() {
  if (accel_downscaled()) {
	// WARNING will not accept values of buffer_s != 32
	// TODO Make this not true
	while (!accel_full()) {
	  n_delay(sample_delay * 16);
	  accel_read_all();
	  n_delay(sample_delay * 16);
	  accel_read_all();
	  gyro_read_all();
	}
	// Wait for hardware buffer to fill up
	n_delay(sample_delay * 16);
  } else {
    while (!accel_full()) {
      n_delay(sample_delay * 32);
      accel_read_all();
      gyro_read_all();
    }
    n_delay(sample_delay * 32);
  }
  // Read long-term from sensors
  if (++long_term_write_num == long_term_write_max) {
    long_term_write_num = 0;
    output_write_data(true);
  } else {
    output_write_data(false);
  }
  accel_reset();
  gyro_reset();
}

// A special version of delay() with
// less accuracy, lower power use
inline void n_delay(long duration) {
  DEND();
  Narcoleptic.delay(duration);
  DBEGIN();
}

