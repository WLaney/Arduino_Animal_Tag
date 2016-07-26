/*
   A test suite for our sensors. Used for testing new devices.
*/
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <ds3234.h>
#include <SFE_MMA8452Q.h>
#include <Light_L3GD20.h>
#include <ctype.h>

#define DBGSTR(s) Serial.println(F(s))

constexpr int cs_sd = 10;
constexpr int cs_rtc = 9;

constexpr int temp102_address = 0x48;

MMA8452Q accel;

/*
   Get the current temperature from TMP102, in Celsius.
*/
float get_temp() {
  Wire.requestFrom(temp102_address, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  short temperatureSum = ((MSB << 8) | LSB) >> 4;
  return temperatureSum * 0.0625;
}

/*
 * Switch SPI so that it can work with either the SD card
 * or real-time clock.
 */
inline void sd_mode() { SPI.setDataMode(SPI_MODE0); }
inline void rtc_mode() { SPI.setDataMode(SPI_MODE1); }

void setup() {
  // General setup
  Serial.begin(9600);
  DBGSTR("Starting Accelerometer...");
  accel.init(SCALE_8G, ODR_12);

  DBGSTR("Starting Gyroscope...");
  Gyro::begin();

  DBGSTR("Starting Wire (Temperature)...");
  Wire.begin();

  DBGSTR("Starting SPI...");
  SPI.begin();

  DBGSTR("Starting RTC");
  rtc_mode();
  DS3234_init(cs_sd, DS3234_INTCN);

  DBGSTR("Starting SD...");
  sd_mode();
  if (!SD.begin(cs_sd)) {
    DBGSTR("SD card could not initialize\n");
  }

  DBGSTR("Pressure isn't implemented. Skipping...");

  // Some test code
}

void loop() {
  // Choose a test(s) to run
  String response;
  DBGSTR("Choose a test to run: \n"
         "  a) All\n"
         "  b) Accelerometer\n"
         "  c) Gyroscope\n"
         "  d) Temperature\n"
         "  e) Real-Time Clock\n"
         "  f) SD Card\n"
         "  g) Pressure (not implemented)\n");
  while (!Serial.available())
    ;
  response = Serial.readString();
  char letter = tolower(response.charAt(0));

  switch (letter) {
    case 'a':
      DBGSTR("All Tests");
      test_accelerometer();
      test_gyroscope();
      test_temperature();
      test_rtc();
      test_sd();
      test_pressure();
      break;
    case 'b':
      test_accelerometer();
      break;
    case 'c':
      test_gyroscope();
      break;
    case 'd':
      test_temperature();
      break;
    case 'e':
      test_rtc();
      break;
    case 'f':
      test_sd();
      break;
    case 'g':
      test_pressure();
      break;
    default:
      DBGSTR("That's not a valid input.");
      break;
  }
}

void test_accelerometer() {
  DBGSTR("Accelerometer");
  run_until_input([] () {
    while (!accel.available())
      ;
    accel.read();
    Serial.print(accel.cx); Serial.write('\t');
    Serial.print(accel.cy); Serial.write('\t');
    Serial.println(accel.cz);
  });
}

void test_gyroscope() {
  DBGSTR("Gyroscope");
  run_until_input([&] () {
    Gyro::l3gd20Data_t data[Gyro::buffer_size];
    fifo_burst_read(data, Gyro::buffer_size);
    for (auto d : data) {
      Serial.print(d.x) ; Serial.write('\t');
      Serial.print(d.y) ; Serial.write('\t');
      Serial.println(d.z);
    }
    delay(2500);
  });
}

void test_temperature() {
  DBGSTR("Temperature");
  run_until_input([]() {
    Serial.println(get_temp());
    delay(1000);
  });
}

void test_rtc() {
  DBGSTR("RTC (not correctly implemented!)");
  rtc_mode();
  run_until_input([]() {
    ts t;
    DS3234_get(cs_rtc, &t);
    Serial.print(t.year); Serial.write('-');
    Serial.print(t.mon);  Serial.write('-');
    Serial.print(t.mday); Serial.write('\t');
    Serial.print(t.hour); Serial.write(':');
    Serial.print(t.min);  Serial.write(':');
    Serial.println(t.sec);
    delay(1000);
  });
}

void test_pressure() {
  DBGSTR("Pressure (not yet implemented!)");
}

void test_sd() {
  DBGSTR("SD (not yet implemented!)");
}

/*
   Run a function continually until Serial input.
   The function only checks for input in between function calls, so
   you can't cancel operation in the middle of a function.

   f takes no arguments, and any arguments it does return are ignored.
*/
template<typename Func> void run_until_input(Func f) {
  DBGSTR("Press any key to continue.");
  while (!Serial.available()) {
    f();
  }
  // Flush Serial buffer
  Serial.readString();
}
