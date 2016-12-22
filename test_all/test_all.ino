/*
   A test suite for our sensors.
   Tests can be run individually or one after the other.
*/
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <ds3234.h>
#include <SFE_MMA8452Q.h>
#include <Light_L3GD20.h>
#include <ctype.h>
#include <SparkFun_MS5803_I2C.h>

#define DBGSTR(s) Serial.println(F(s))

constexpr int cs_sd = 10;
constexpr int cs_rtc = 9;

constexpr int temp102_address = 0x48;

MMA8452Q accel;
MS5803 pressure(ADDRESS_LOW);

Sd2Card card;
SdVolume volume;
SdFile root;

/*
   Get the current temperature from TMP102, in Celsius.
*/
inline float get_temp() {
  Wire.requestFrom(temp102_address, 2);
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  short temperatureSum = ((MSB << 8) | LSB) >> 4;
  return temperatureSum * 0.0625;
}

/*
   Switch SPI so that it can work with either the SD card
   or real-time clock.
*/
inline void sd_mode() {
  SPI.setDataMode(SPI_MODE0);
}
inline void rtc_mode() {
  SPI.setDataMode(SPI_MODE1);
}

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
  DS3234_init(cs_rtc, DS3234_INTCN);

  DBGSTR("Starting SD...");
  sd_mode();
  if (!SD.begin(cs_sd)) {
    DBGSTR("SD card could not initialize\n");
  }

  DBGSTR("Starting pressure...");
  pressure.begin();
}

void loop() {
  String response;
  DBGSTR("Choose a test to run: \n"
         "  a) All\n"
         "  b) Accelerometer\n"
         "  c) Gyroscope\n"
         "  d) Temperature\n"
         "  e) Real-Time Clock\n"
         "  f) SD Card\n"
         "  g) Pressure (not tested)\n");
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
  DBGSTR("Gyroscope:\n" \
         "(x, y, z) values will appear in short bursts.\n" \
         "These values should be within the maximum range of the gyroscope (currently 250DPS).");
  run_until_input([&] () {
    Gyro::l3gd20Data_t data[Gyro::buffer_size];
    fifo_burst_read(data, Gyro::buffer_size);
    for (auto d : data) {
      Serial.print(Gyro::s2f(d.x)) ; Serial.write('\t');
      Serial.print(Gyro::s2f(d.y)) ; Serial.write('\t');
      Serial.println(Gyro::s2f(d.z));
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
  DBGSTR("RTC: Displays current time once per second.");
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
  DBGSTR("Pressure\n" \
         "Should display temperature and pressure.");
  run_until_input([] () {
    float pres = pressure.getPressure(ADC_4096);
    float temp = pressure.getTemperature(CELSIUS, ADC_4096);
    DBGSTR("Pressure: "); Serial.println(pres);
    DBGSTR("Temperature (C): "); Serial.println(temp);
    delay(1000);
  });
}

void test_sd() {
  DBGSTR("SD\n" \
         "The SD test code is lifted directly from the CardInfo sketch " \
         "you're used to working with.");

  sd_mode();
  if (!card.init(SPI_HALF_SPEED, cs_sd)) {
    DBGSTR("Initialization failed. Check the card, the wiring, chip select pin, etc.");
    return;
  } else {
    DBGSTR("Wiring is correct and a card is present.");
  }

  DBGSTR("Card type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      DBGSTR("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      DBGSTR("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      DBGSTR("SDHC");
      break;
    default:
      DBGSTR("Unknown");
  }

  if (!volume.init(card)) {
    DBGSTR("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card correctly");
    return;
  }

  uint32_t volumesize;
  DBGSTR("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                         // SD card blocks are always 512 bytes
  DBGSTR("Volume size (bytes): ");
  Serial.println(volumesize);

  Serial.println();
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

/*
   Run a function continually until Serial input.
   The function only checks for input in between function calls, so
   you can't cancel operation in the middle of a function.

   f takes no arguments, and any arguments it does return are ignored.
*/
template<typename Func> void run_until_input(Func f) {
  DBGSTR("Press any key to stop the test.");
  while (!Serial.available()) {
    f();
  }
  // Flush Serial buffer
  Serial.readString();
}
