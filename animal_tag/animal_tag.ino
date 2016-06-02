#include <Wire.h>
#include <SPI.h>          // serial, sd card
#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping
#include "debug.hpp"
#include "buffer.hpp"
#include "rtc.hpp"
#include "temp.hpp"

#define CS_SD 10

void setup()
{
  // Open serial communications and wait for port to open:
  DBEGIN();

  //DO SETUP HERE
  buffer_setup();
  temp_setup();
  rtc_setup();
  
  // Setup SD card
  sd_mode();
  pinMode(10, OUTPUT);
  SD.begin(CS_SD);

  // Setup wire
  Wire.begin();
  
  // Write startup information
  rtc_mode();
  rtc_update();
  sd_mode();
  File f = SD.open("data.txt", FILE_WRITE);
  // Need to wait for SD to start working for some reason
  delay(100);
  if (f) {
    f.print("\t\t\t");
    rtc_write(f);
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

  // Keep updating until we run out of buffer space
  while (!buffer_needs_write()) {
    buffer_update();
  }
  flush_and_write();
  
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
    rtc_update();
    temp_update();
  }
  
  //Write to SD
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    buffer_write(file);
    // Long-term
    if (write_num == 0) {
      file.print("\t\t\t\t\t\t");
      rtc_write(file);
      file.print('\t');
      temp_write(file);
      file.write("\n");
    }
    file.close();
    DBGLN("sd data written");
  }
}

inline void sd_mode() {
  SPI.setDataMode(SPI_MODE0);
}

inline void rtc_mode() {
  SPI.setDataMode(SPI_MODE1);
}
