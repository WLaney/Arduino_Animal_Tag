/*
 * This sketch opens up a file and writes to it. Continuously. Like, nothing but writing.
 * If it ever gets to 512MB (a conservative file size limit), it deletes the file and restarts.
 */
#include <SD.h>

#define DBGSTR(s) Serial.print(F(s))

const char *file_name = "trash.dat";

void setup() {
	Serial.begin(9600);
	bool good = SD.begin(10);
	if (!good) {
		DBGSTR("SD card not initialized.\n");
		while (true)
			;
	}
	DBGSTR("Card initialized.\n");
}

void loop() {
	DBGSTR("Starting file ");
	Serial.println(file_name);

	SD.remove(file_name);
	File out = SD.open(file_name, FILE_WRITE);
	if (out) {
		// 512MB / 512 = 1024 * 1024 = 2 ^ 20
		// So we use 3 loops: 2^8, 2^8, and 2^4
		byte i, j, k;
		for (i=0; i<15; i++) {
			for (j=0; j<255; j++) {
				for (k=0; k<255; k++) {
					// Haha, what
					out.write((byte *) 0x01, 512);
					out.flush();
				}
			}
		}
		out.close();
	} else {
		DBGSTR("Can't open the file!");
		while (true)
			;
	}
}
