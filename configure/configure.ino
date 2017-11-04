/*
 * Write configuration data to the device's EEPROM - its name, its
 * sensor orientation, etc.
 * 
 * Tag EEPROM is of the format:
 * 0x0..0x3 - Device name, e.g. "SHRK".
 * 0x4      - Sensor orientation.
 *            * 0 - Accelerometer and gyroscope have the same orientation.
 *            * 1 - Gyroscope needs rotation (see MATLAB sketch).
 */
#include <EEPROM.h>
#include "tag.h"
#include <DSRTCLib2.h>
#define PRINTSTR(S) Serial.print(F(S))

Tag tag;
DSRTCLib RTC = DSRTCLib();

void setup() {
  Serial.begin(9600);
  tag = Tag(true);
}

void loop() {
  PRINTSTR("Current Tag Configuration:\n");
  print_configuration();
  PRINTSTR(
    "\nMENU\n" \
    "a) Configure everything (for new tags)\n" \
    "b) Configure something specific\n" \
    "c) Set or read the current time\n" \
    "d) Reset the EEPROM (for testing only)\n\n"
  );
  while (!Serial.available())
    ;
  char result = Serial.read();
  if (result < 'a') result = (result - 'A') + 'a';
  switch (result) {
  case 'a':
    set_name();
    set_accel_scale();
    set_gyro_scale();
    set_odr();
    set_orientation();
    set_hq_accel();
    set_startup_delay();
    calibrate();
    write_changes();
    break;
  case 'b':
    PRINTSTR("Changes WILL NOT BE SAVED until you write them to EEPROM.\n");
    config_menu();
    break;
  case 'c':
    menu_time();
    break;
  case 'd':
    tag_reset();
    write_changes();
    break;
  default:
    PRINTSTR("Invalid option.\n");
    break;
  }
}

void config_menu() {
  while (true) {
    PRINTSTR(
      "a) Print current device configuration\n" \
      "b) Set device name\n" \
      "c) Set accelerometer scale\n" \
      "d) Set gyroscope scale\n" \
      "e) Set samplerate\n" \
      "f) Set device orientation (for old tags)\n" \
      "g) Set high-quality accelerometer\n" \
      "h) Set alarm startup time\n" \
      "i) Calibrate gyroscope (not implemented)\n" \
      "j) Write changes\n" \
      "k) Exit\n\n"
    );
    while (!Serial.available())
      ;
    char result = Serial.read();
    // Somewhat janky downcase() method
    if (result < 'a') result = (result - 'A') + 'a';
    switch (result) {
    case 'a':
      print_configuration();
      break;
    case 'b':
      set_name();
      break;
    case 'c':
      set_accel_scale();
      break;
    case 'd':
      set_gyro_scale();
      break;
    case 'e':
      set_odr();
      break;
    case 'f':
      set_orientation();
      break;
    case 'g':
      set_hq_accel();
      break;
    case 'h':
      set_startup_delay();
      break;
    case 'i':
      calibrate();
      break;
    case 'j':
      write_changes();
      break;
    case 'k':
      return;
    default:
      PRINTSTR("Sorry, didn't quite catch that...");
    }
    Serial.println('\n');
  }
}

void menu_time() {
  while (true) {
    PRINTSTR(
      "a) Read time\n" \
      "b) Write time\n" \
      "c) Exit\n"
    );
    while (!Serial.available())
      ;
    char r = Serial.read();
    switch (r) {
    case 'a':
      menu_read_time();
      break;
    case 'b':
      menu_write_time();
      break;
    case 'c':
      return;
    default:
      PRINTSTR("Invalid input\n");
      break;
    }
  }
}

void menu_read_time() {
  PRINTSTR("Press any key to exit to the previous menu.\n");
  byte r[1];
  r[0] = 255;
  while (r[0] == 255) {
    read_and_print_time();
    Serial.println();
    // 1000ms timeout acts as a natural delay
    Serial.readBytes(r, 1);
  }
  Serial.println();
}

void menu_write_time() {
  PRINTSTR("Enter the current time (YYYY-MM-DD HH:MM:SS): ");
  while (!Serial.available())
    ;
  RTC.setYears(Serial.parseInt());
  // Throw out -, which the script would see as a minus sign
  Serial.read();
  RTC.setMonths(Serial.parseInt());
  Serial.read();
  RTC.setDays(Serial.parseInt());
  RTC.setHours(Serial.parseInt());
  RTC.setMinutes(Serial.parseInt());
  RTC.setSeconds(Serial.parseInt());
  RTC.writeTime();
  read_and_print_time();
  Serial.println();
}

/*
 * Print time via the format
 * YYYY-MM-DD hh:mm:ss
 */
void read_and_print_time() {
  RTC.readTime();
  print_time(
    RTC.getSeconds(),
    RTC.getMinutes(),
    RTC.getHours(),
    RTC.getDays(),
    RTC.getMonths(),
    RTC.getYears()
  );
}

void print_time(byte sec, byte min, byte hour, byte day, byte mon, int year) {
  Serial.print(year); Serial.write('-');
  print_zpad(mon); Serial.write('-');
  print_zpad(day); Serial.write(' ');
  print_zpad(hour); Serial.write(':');
  print_zpad(min); Serial.write(':');
  print_zpad(sec);  
}

/*
 * Prints bytes zero-padded to two zeros.
 */
void print_zpad(byte d) {
  if (d < 10)
    Serial.write('0');
  Serial.print(d);
}

// Update the current settings here
void get_configuration() {
  tag.update();
}

// Print the tag's current values
void print_configuration() {
  PRINTSTR("Name:   "); Serial.write(tag.name, 4); Serial.write('\n');
  PRINTSTR("Orient: "); Serial.println(tag.orient);
  PRINTSTR("X Bias: "); Serial.println(tag.bias_x);
  PRINTSTR("Y Bias: "); Serial.println(tag.bias_y);
  PRINTSTR("Z Bias: "); Serial.println(tag.bias_z);
  PRINTSTR("Accel Scale: ");
  switch (tag.accel_scale) {
    case Tag::ACCEL_2G: PRINTSTR("2G\n"); break;
    case Tag::ACCEL_4G: PRINTSTR("4G\n"); break;
    case Tag::ACCEL_8G: PRINTSTR("8G\n"); break;
    default:
      PRINTSTR("UKNOWN VALUE - "); Serial.println(tag.accel_scale);
  }
  PRINTSTR("Gyro Scale:  ");
  switch (tag.gyro_scale) {
    case Tag::GYRO_250DPS:  PRINTSTR("250DPS\n"); break;
    case Tag::GYRO_500DPS:  PRINTSTR("500DPS\n"); break;
    case Tag::GYRO_1000DPS: PRINTSTR("1000DPS\n"); break;
    case Tag::GYRO_2000DPS: PRINTSTR("2000DPS\n"); break;
    default:
      PRINTSTR("UNKNOWN VALUE - "); Serial.println(tag.gyro_scale);
  }
  PRINTSTR("Samplerate:  ");
  switch (tag.sample_rate) {
    case Tag::ODR_12_5_HZ: PRINTSTR("12.5Hz\n"); break;
    case Tag::ODR_25_HZ:   PRINTSTR("25Hz\n"); break;
    case Tag::ODR_50_HZ:   PRINTSTR("50Hz\n"); break;
    default:
      PRINTSTR("UNKNOWN VALUE - "); Serial.println(tag.sample_rate);
  }
  PRINTSTR("HQ Accelerometer: ");
  Serial.println(tag.accel_hq);
  PRINTSTR("Alarm startup delay (seconds): ");
  Serial.println(tag.startup_delay);
}

void set_name() {
  PRINTSTR(
    "TAG NAME\n========\n" \
    "The name for the device.\n" \
    "The name is on the bottom of the tag; just remove the . and v;\n" \
    "For example, v2.1a is 21a.\n\n" \
    "Enter tag name (up to 4 letters): "
  );
  // Wait for the user to enter 1 byte.
  // Then wait about an eighth of a second for 3 more bytes to complete the name.
  tag.name[0] = ' ';
  tag.name[1] = ' ';
  tag.name[2] = ' ';
  tag.name[3] = ' ';
  while (!Serial.available())
    ;
  Serial.readBytes(tag.name, 4);
  Serial.write((byte *) &tag.name, 4);
  Serial.println();
}

void set_accel_scale() {
  PRINTSTR(
    "ACCEL SCALE\n==========\n" \
    "The max range of the accelerometer, in g's.\n" \
    "One g is 9.81 meters per second.\n" \
    "  a) 2g\n" \
    "  b) 4g\n" \
    "  c) 8g\n"
  );
  while (!Serial.available())
   ;
  char r = Serial.read();
  Tag::ACCEL_SCALE a;
  switch (r) {
  case 'a':
    a = Tag::ACCEL_2G;
    break;
  case 'b':
    a = Tag::ACCEL_4G;
    break;
  case 'c':
    a = Tag::ACCEL_8G;
    break;
  default:
    PRINTSTR("Not a valid option");
    return;
  }
  tag.accel_scale = a;
  Serial.println();
}

void set_gyro_scale() {
  PRINTSTR(
    "GYRO SCALE\n==========\n" \
    "Max range of the gyroscope, in degrees per second.\n"
    "a) 250DPS\n" \
    "b) 500DPS\n" \
    "c) 1000DPS\n" \
    "d) 2000DPS\n"
  );
  while (!Serial.available())
   ;
  char r = Serial.read();
  switch (r) {
  case 'a':
    tag.gyro_scale = Tag::GYRO_250DPS;
    break;
  case 'b':
    tag.gyro_scale = Tag::GYRO_500DPS;
    break;
  case 'c':
    tag.gyro_scale = Tag::GYRO_1000DPS;
    break;
  case 'd':
    tag.gyro_scale = Tag::GYRO_2000DPS;
    break;
  default:
    PRINTSTR("Not a valid option\n");
  }
}

void set_odr() {
  PRINTSTR(
    "OUTPUT DATA RATE\n================\n" \
    "Rate at which accelerometer and gyroscope data are taken.\n" \
    "Higher values use more power, but are more accurate.\n" \
    "a) 12.5Hz\n" \
    "b) 25Hz\n" \
    "c) 50Hz\n"
  );
  while (!Serial.available())
   ;
  char r = Serial.read();
  switch (r) {
  case 'a':
    tag.sample_rate = Tag::ODR_12_5_HZ;
    break;
  case 'b':
    tag.sample_rate = Tag::ODR_25_HZ;
    break;
  case 'c':
    tag.sample_rate = Tag::ODR_50_HZ;
    break;
  default:
    PRINTSTR("Not a valid option\n");
  }
}

void set_orientation() {
  PRINTSTR(
    "ORIENTATION\n=========\n" \
    "This is an ancient legacy feature, and should be set to 0 on all new tags.\n"
  );
  bool valid = question("Is the device an early prototype with an incorrect orientation? (y/n)\n");
  tag.orient = (byte)valid;
}

void set_hq_accel() {
  PRINTSTR(
    "HQ ACCEL\n========\n" \
    "If enabled, the accelerometer uses more reads for sampling.\n" \
    "Provides more accurate (and power-intensive) results.\n"
  );
  bool hq = question("Do you want to let the accelerometer use high-quality oversampling? (y/n)\n");
  tag.accel_hq = hq;
}

void set_startup_delay() {
  PRINTSTR(
    "STARTUP DELAY\n=============\n" \
    "The time for which the tag should sleep before activating.\n" \
    "The tag uses very little power in this state.\n\n" \
    "Enter startup delay in seconds: "
  );
  while (!Serial.available())
    ;
  tag.startup_delay = Serial.parseInt();;
}

void tag_reset() {
  PRINTSTR("WARNING: this will prevent your tag from working until you configure it again.\n");
  if (question("Are you sure you want to this? (y/n)\n")) {
    PRINTSTR("Don't say I didn't warn you.\n");
    tag.reset();
  } else {
    PRINTSTR("Smart choice.\n");
  }
}

void calibrate() {
  PRINTSTR("CALIBRATION\n===========\nAn unused feature originally for gyroscope calibration. Currently not used.");
}

void write_changes() {
  PRINTSTR("Writing values to EEPROM...\n");
  tag.bias_x = 0.0F;
  tag.bias_y = 0.0F;
  tag.bias_z = 0.0F;
  tag.write();
  PRINTSTR("Saved.\n");
}

// Ask a (y/n) question and return the response
bool question(const char *msg) {
  Serial.print(msg);
  while (true) {
    while (!Serial.available())
      ;
    switch (Serial.read()) {
      case 'Y':
      case 'y':
      PRINTSTR("Yes\n");
    return true;
        break;
      case 'N':
      case 'n':
      PRINTSTR("No\n");
    return false;
      break;
      default:
        PRINTSTR("Invalid input.\n");
    }
  }
}

