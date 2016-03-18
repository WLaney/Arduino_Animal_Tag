//include libraries
//=====================================
#include <Wire.h>         // temperature, accelerometer; i2c
#include <SFE_MMA8452Q.h> // accelerometer
#include <SPI.h>          // serial, rtc, sd card
#include "ds3234.h"       // rtc
#include <SD.h>           // sd card

// SD Card vars
//=====================================
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10; // chip select pin for SD

// RTC Vars
//=====================================
const int cs = 9; // chip select pin for RTC

// Accelerometer vars
//=====================================
MMA8452Q accel;

// Temp sensor vars
//=====================================
int tmp102Address = 0x48;

// Struct for accelerometer data.
struct accel_data {
  float x, y, z;
};

// Our buffer will take up to N bytes of memory.
const int buff_max_length = 256 / sizeof(accel_data);
accel_data buff[buff_max_length];
size_t buff_length = 0;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Begin RTC
  DS3234_init(cs, DS3234_INTCN);

  //begin SD card
  //=====================================
  SPI.setDataMode(SPI_MODE0); // switch mode to SD
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  // intialize card:
  SD.begin(chipSelect);

  //begin I2c
  //=====================================
  Wire.begin();

  //begin accel
  //=====================================
  accel.init(SCALE_8G, ODR_6);

  // Write the datetime that this was opened
  ts t;
  float celsius;
  SPI.setDataMode(SPI_MODE1);
  celsius = get_long_term(&t);
  SPI.setDataMode(SPI_MODE0);
  File f = SD.open("data.txt", FILE_WRITE);
  if (f) {
    write_long_term(f, celsius, t);
    f.close();
    Serial.println("Wrote SD at start");
  }
  //confrim that the everything is working and there is serial communication
  Serial.println("setup done");

}

void loop() {
  //data writen as tab seprated values in order of
  //time (month\day\year Hr:min:sec), temp, accel in x,  accel in y,  accel in z
  //print time data
  
  // If we're out of space, write to the SD card
  if (buff_length == buff_max_length) {
    flush_and_write();
  } else {
    // Get the value that we're working on
    accel_data *value = &buff[buff_length];
    accel.read();
    value->x = accel.cx;
    value->y = accel.cy;
    value->z = accel.cz;

    Serial.print(value->x);
    Serial.print("\t");
    Serial.print(value->y);
    Serial.print("\t");
    Serial.print(value->z);
    Serial.println("");

    buff_length++;
  }
  //wait 80ms (approx 12Hz) before begingin the loop again
  delay(80);
}

// Flush the buffer to the SD card, writing temperature and time
// data as you do.
void flush_and_write()
{
  SPI.setDataMode(SPI_MODE1); //switch to RTC mode
  float celsius;
  ts t;
  celsius = get_long_term(&t);
  
  //Write data to SD
  //=====================================
  SPI.setDataMode(SPI_MODE0);  // switch mode to SD
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    // Accelerometer data
    for (accel_data &d : buff) {
      dataFile.print(d.x);
      dataFile.print("\t");
      dataFile.print(d.y);
      dataFile.print("\t");
      dataFile.println(d.z);
    }
    write_long_term(dataFile, celsius, t);
    dataFile.close();
    Serial.println("sd data written");
  }
  // Reset the buffer to the beginning
  buff_length = 0;
}

// Returns celsius as a float and sets t to the current time
// SPI must be in SPI_MODE1
float get_long_term(ts *t) {
  //Read RTC
  DS3234_get(cs, t);
  Serial.print(t->hour);
  Serial.print(":");
  Serial.print(t->min);
  Serial.print(":");
  Serial.println(t->sec);
  
  // read celsius
  Wire.requestFrom(tmp102Address, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4;
  return TemperatureSum * 0.0625;
}

// Write this data to the SD card
// SPI must be SPI_MODE0
void write_long_term(File f, float celsius, ts t) {
    // Time
    f.print(t.mon);
    f.print("/");
    f.print(t.mday);
    f.print("/");
    f.print(t.year);
    f.print("\t");
    f.print(t.hour);
    f.print(":");
    f.print(t.min);
    f.print(":");
    f.print(t.sec);
    f.print("\t");
    // Temperature
    f.println(celsius);
}
