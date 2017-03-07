#include <Wire.h>
/*
 * A simple device registry editor. Originally made for the FXAS21002C gyro,
 * edited for the MMA8451Q accelerometer
 */

byte readReg(byte);
void writeReg(byte, byte);

int parseHex(char *, size_t);

void printHelp();

// Device address - currently for Accel_1Q
byte addr = 0x1d;

// Main code
void setup() {
	Serial.begin(9600);
	Serial.setTimeout(100);
	
	Wire.begin();
	// Halt unless device found
	Wire.beginTransmission(addr);
	byte res = Wire.endTransmission();
	if (res != 0) {
		Serial.print(F("The device at address "));
		Serial.print((short) addr, HEX);
		Serial.println(" could not be found.");
		while (true)
			;
	}
	printHelp();
}

void loop() {
	byte ri, vi;
	byte r, v;
	String in;
	
	while (!Serial.available())
		delay(100);
		
	in = Serial.readString();
	switch (in[0]) {
	case 'r':
		ri = in.indexOf(' ');
		r = parseHex(in.c_str() + ri + 1, 2);
		readReg(r);
		break;
	case 'w':
		ri = in.indexOf(' ');
		vi = in.lastIndexOf(' ');
		r = parseHex(in.c_str() + ri + 1, 2);
		v = parseHex(in.c_str() + vi + 1, 2);
		writeReg(r, v);
		break;
	default:
		Serial.println(F("Unrecognized command."));
		// pass-through
	case '?':
		printHelp();
		break;
	}
}

/*
 * Return the value of a single register (verbosely)
 */
byte readReg(byte reg) {
	byte tres, data = 0xFF;
	
	Serial.print(F("read "));
	Serial.println((short) reg, HEX);
	// Get address
	Serial.println(F("Send register address..."));
	Wire.beginTransmission(addr);
	Wire.write(reg);
	tres = Wire.endTransmission(false);
	// Read data
	if (tres == 0) {
		Serial.println(F("Read data"));
		Wire.requestFrom(addr, 1);
		while (Wire.available() < 1)
			;
		data = Wire.read();
		Wire.endTransmission(true);
	}
	// After-action report
	Serial.print("Address transmission ");
	switch (tres) {
	case 0: Serial.println(F("SUCCESS")); break;
	case 1: Serial.println(F("FAILED: Data too long to fit buffer")); break;
	case 2: Serial.println(F("FAILED: Received NACK on address transmit")); break;
	case 3: Serial.println(F("FAILED: Received NACK on data transmit")); break;
	case 4: Serial.println(F("FAILED: Other error")); break;
	}
	if (tres == 0) {
		Serial.print("Read ");     Serial.print((short) data, HEX);
		Serial.print(" (binary "); Serial.print((short) data, BIN);
		Serial.println(")");
	}
	
	return data;
}

/*
 * Write to a single register
 */
void writeReg(byte reg, byte val) {
	Serial.print(F("write ")); Serial.print((short) reg, HEX);
	Serial.print(" ");  Serial.println((short) val, HEX);
	// Send address and data in one write
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission();
	Serial.println(F("Done."));
}

void printHelp() {
	Serial.println(F(
		"REGISTER EDITOR\n" \
		"All inputs/outputs are in hex (FF, not 0xFF)\n" \
		"Refer to your device's datasheet for regs\n\n" \
		"    r <reg>       - Get the value of <reg>\n" \
		"    w <reg> <val> - Set the value of <reg> to <val>\n" \
		"    ?             - Display this help message\n"
	));
}

/*
 * Turn a string containing only a positive hex value into an integer
 * Bad input causes it to return what it's accumulated so far
 */
int parseHex(char *s, size_t t) {
	int sum = 0;
	int i;
	for (i=0; i<t; i++) {
		char d = s[i];
		char h;
		if      (d >= 'a' && d <= 'f') h = 10 + d - 'a';
		else if (d >= 'A' && d <= 'F') h = 10 + d - 'A';
		else if (d >= '0' && d <= '9') h = d - '0';
		else break;
		
		sum <<= 4;
		sum += h;
	}
	return sum;
}
