/*
 * Writes a blank file to the SD card. If that file already
 * exists, it adds a suffix and tries again, incrementing the
 * suffix until it works.
 */
#include <SD.h>

/*
 * The SD library uses an 8.3 format - 8 characters for
 * the name and 3 characters for the extension. For
 * that reason, we use the format DATA-XXX.SRK.
 * 
 * For now, let's limit the maximum number of files to
 * 255. If the number is 255, let's put MAX instead.
 */
char *file_name = "DATA-XXX.SRK";

constexpr byte cs_sd = 10;

void setup() {
  Serial.begin(9600);
  if (!SD.begin(cs_sd)) {
    Serial.println(F("Couldn't open SD card."));
    while (true)
      ;
  }
}

void loop() {
  Serial.println(F("Setting filename..."));
  set_file_name(file_name);
  //Serial.print(F("Set name to "));
  Serial.println(file_name);

  File f = SD.open(file_name, FILE_WRITE);
  if (f) {
    f.write("HI!");
    f.close();
  } else {
    Serial.println(F("Couldn't open the file!"));
  }
  delay(500);
}

/*
 * Tries to set c to an unused filename of the format DATA-XXX.SRK,
 * 
 */
void set_file_name(char *c) {
  c[0]='D'; c[1]='A'; c[2]='T'; c[3]='A';
  // c[5..7] are set below
  c[8]='.'; c[9]='S'; c[10]='R'; c[11]='K'; c[12]='\0';
  
  // Find the lowest number to use (max 255)
  unsigned char n = 0;
  do {
    c[5] = '0' + (n / 100) % 10;
    c[6] = '0' + (n / 10) % 10;
    c[7] = '0' + n % 10;
    if (!SD.exists(c)) {
      break;
    }
    n++;
  } while (n != 0);
  
  if (n == 255) {
    c[5] = 'M';
    c[6] = 'A';
    c[7] = 'X';
  }
}

