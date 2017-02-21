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

SAMPLE_RATE sample_rate;
byte sample_delay;

void setup()
{
  header_data header;
  FXAS::Range gscale;
  FXAS::ODR godr;
  MMA8452Q_Scale ascale;
  MMA8452Q_ODR aodr;
  SAMPLE_RATE odr;
  
  // Get configuration from EEPROM
  for (byte i=0; i<4; i++) {
    header.name[i] = EEPROM.read(i);
  }
  header.orient = EEPROM.read(4);
  EEPROM.get(5,  header.gx); // gyroscope bias
  EEPROM.get(9,  header.gy);
  EEPROM.get(13, header.gz);
  ascale = static_cast<MMA8452Q_Scale>(EEPROM.read(17));
  gscale = static_cast<FXAS::Range>(EEPROM.read(18));
  odr = static_cast<SAMPLE_RATE>(EEPROM.read(19));
  switch (odr) {
	case ODR_12_5_HZ:
		aodr = ODR_12;
		godr = FXAS::ODR::HZ_12_5;
		sample_delay = byte{1000.0 / 12.5} - 10;
		break;
	case ODR_25_HZ:
		aodr = ODR_50;
		godr = FXAS::ODR::HZ_25;
		sample_delay = byte{1000.0 / 25.0} - 10;
		break;
	case ODR_50_HZ:
		aodr = ODR_50;
		godr = FXAS::ODR::HZ_50;
		sample_delay = byte{1000.0 / 50.0} - 10;
		break;
	default:
		DBGSTR("Invalid sample rate; assuming you meant 25Hz");
		aodr = ODR_50;
		godr = FXAS::ODR::HZ_25;
		sample_delay = byte{1000.0 / 25.0} - 10;
  }
  
  // Debug commands won't show up if they're
  // turned off in debug.h
  DBEGIN();
  accel_setup(SCALE_2G, ODR_50); // not a misprint, accel can't do 25Hz
  gyro_setup(FXAS::Range::DPS_250, FXAS::ODR::HZ_25);
  temp_setup();
  rtc_setup();
  
  // Start SD card
  if (output_setup()) {
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
  
  // Write header
  // Accelerometer, Gyroscope Write Size
  header.aws = accel_write_size();
  header.gws = gyro_write_size();
  // Write period (long_term_write_max)
  header.period = long_term_write_max;
  // Scaling
  header.as = accel_scale();
  header.gs = gyro_scale();
  output_write_header(header);
}

void loop() {
  // We hold the first N gyroscope values in the software buffer; the rest
  // are kept in the hardware buffer
  for (byte i=0; i<gyro_size(); i++) {
    accel_read();
    n_delay(sample_delay);
  }
  if (gyro_is_active()) {
    gyro_read_all();
  }
  while (!accel_full()) {
    accel_read();
    n_delay(sample_delay);
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

