#include <DSRTCLib2.h>

DSRTCLib RTC = DSRTCLib();

#define PRINTSTR(S) Serial.print(F(S))

void setup() {
	Serial.begin(9600);
}

void loop() {
  menu_time();
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

