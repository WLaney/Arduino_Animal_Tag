#include <Wire.h>         // temperature, accelerometer; i2c
#include <SFE_MMA8452Q.h> // accelerometer
#include <SPI.h>          // serial, sd card

#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping

#include "debug.h"
#include "Sensor.hpp"

#include "RTCSensor.hpp"
#include "AccelSensor.hpp"
#include "TempSensor.hpp"

// Chip Select
//=====================================
const int cs_sd = 10; // chip select pin for SD

RTCSensor rtc;
AccelSensor accel;
TempSensor temp;

void setup()
{
  // Open serial communications and wait for port to open:
  DBEGIN();

  rtc.setup();
  accel.setup();
  temp.setup();
  
  // Setup SD card
  sd_mode();
  pinMode(10, OUTPUT);
  SD.begin(cs_sd);

  // Setup wire
  Wire.begin();
  
  // Write startup information
  rtc_mode();
  rtc.read();
  sd_mode();
  File f = SD.open("data.txt", FILE_WRITE);
  // Need to wait for SD to start working for some reason
  delay(100);
  if (f) {
    f.print("\t\t\t");
    rtc.flush(f);
    f.write('\n');
    f.close();
    DBGLN("SD written");
  }
  //confrim that the everything is working and there is serial communication
  DBGLN("setup done");
}

void loop() {
  //data writen as tab seprated values in order of
  //time (month\day\year Hr:min:sec), temp, accel in x,  accel in y,  accel in z
  //print time data
  
  // If we're out of space, write to the SD card
  if (accel.needsFlush()) {
    flush_and_write();
  } else {
    accel.read();
  }
  //wait 80ms (approx 12Hz) before beginning the loop again
  DEND();
  Narcoleptic.delay(80);
  DBEGIN();
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
    rtc.read();
    temp.read();
  }
  
  //Write to SD
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    accel.flush(file);
    // Long-term
    if (write_num == 0) {
      file.print("\t\t\t");
      rtc.flush(file);
      file.print('\t');
      temp.flush(file);
      file.write("\n");
    }
    file.close();
    DBGLN("sd data written");
  }

  accel.reset();
}

inline void sd_mode() {
  SPI.setDataMode(SPI_MODE0);
}

inline void rtc_mode() {
  SPI.setDataMode(SPI_MODE1);
}
