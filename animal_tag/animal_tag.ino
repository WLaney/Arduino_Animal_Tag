#include <Wire.h>         // temperature, accelerometer; i2c
#include <SFE_MMA8452Q.h> // accelerometer
#include <SPI.h>          // serial, rtc, sd card
#include "ds3234.h"       // rtc
#include <SD.h>           // sd card
#include <Narcoleptic.h>  // sleeping

#define DEBUG // Comment this line out to remove Serial prints
#ifdef DEBUG
  #define DBEGIN() Serial.begin(9600)
  #define DEND()   Serial.end()
  #define DBG(s) Serial.print(s)
  #define DBGLN(s) Serial.println(s)
#else
  #define DBEGIN()
  #define DEND()
  #define DBG(s)
  #define DBGLN(s) 
#endif

class Sensor {
  virtual void setup() = 0;
  virtual void read()  = 0;
  virtual void flush(File sd) = 0;
};

class RTCSensor : Sensor {
private:
  const int cs = 9;
  ts t;
public:
  void setup() {
    DS3234_init(cs, DS3234_INTCN);
  }

  void read() {
    DBG("Read time: ");
    DS3234_get(cs, &t);
    DBG(t.hour);
    DBG(":");
    DBG(t.min);
    DBG(":");
    DBGLN(t.sec);
  }

  // prints "YYYY:MM:DD hh:mm:ss"
  void flush(File sd) {
    DBGLN("Wrote time");
    sd.print(t.year);
    sd.print("-");
    sd.print(t.mon);
    sd.print("-");
    sd.print(t.mday);
    sd.print("\t");
    sd.print(t.hour);
    sd.print(":");
    sd.print(t.min);
    sd.print(":");
    sd.print(t.sec);
  }
};

class AccelSensor: Sensor {
private:
  MMA8452Q accel;
  struct accel_data {
    float x, y, z;
  };
  const static int buff_max_length = 256 / sizeof(accel_data);
  accel_data buff[buff_max_length];
  size_t buff_length = 0;
public:
  void setup() {
    accel.init(SCALE_8G, ODR_6);
  }
  
  void read() {
    // ADD INVARIANT ASSERTION HERE
    accel.read();
    accel_data *v = &buff[buff_length];
    v->x = accel.cx;
    v->y = accel.cy;
    v->z = accel.cz;

    DBG(v->x);
    DBG("\t");
    DBG(v->y);
    DBG("\t");
    DBG(v->z);
    DBG("\n");

    buff_length++;
  }

  boolean needsFlush() {
    return (buff_length >= buff_max_length);
  }

  // Prints "x y z"
  void flush(File sd) {
    for (accel_data &d : buff) {
      sd.print(d.x);
      sd.print("\t");
      sd.print(d.y);
      sd.print("\t");
      sd.print(d.z);
    }
  }
};

class TempSensor : Sensor {
private:
  const int tmp102Address = 0x48;
  float celsius;
public:
  void setup() { }

  void read() {
    Wire.requestFrom(tmp102Address, 2);
    byte MSB = Wire.read();
    byte LSB = Wire.read();
    int temperatureSum = ((MSB << 8) | LSB) >> 4;
    celsius = temperatureSum * 0.0625;
  }

  void flush(File sd) {
    sd.println(celsius);
  }
};

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
  if (f) {
    rtc.flush(f);
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
  const int write_max = 10;
  
  if (write_num++ == write_max) {
    write_num = 0;
    rtc_mode();
    rtc.read();
    temp.read();
  }
  
  //Write data to SD
  sd_mode();
  File file = SD.open("data.txt", FILE_WRITE);
  if (file) {
    accel.flush(file);
    file.print('\n');
    if (write_num == 0) {
      file.print("\t\t\t");
      rtc.flush(file);
      file.print('\t');
      temp.flush(file);
      file.print('\n');
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
