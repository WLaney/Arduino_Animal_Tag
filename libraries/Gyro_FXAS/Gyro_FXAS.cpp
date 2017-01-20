#include "Gyro_FXAS.h"
#include "Arduino.h"
#include "Wire.h"
#include <stddef.h>

namespace FXAS {
	
	static byte readReg(Register);
	static void writeReg(Register, byte);
	
	// Value factory-coded into WHO_AM_I register
	constexpr byte whoAmIValue = 0xD7;
	
	Range currentRange = Range::DPS_250;
	
    /*
     * Start the gyroscope in standby mode.
     * This returns whether or not the gyro could be successfully reached.
     */
	bool begin(ODR odr, Range range, bool burst) {
		// TODO Reset FIFO somehow
		currentRange = range;
		Wire.begin();
		// Go into standby mode
		writeReg(Register::CTRL_REG1, 0x00);
		// Check WHO_AM_I byte
		byte wai = readReg(Register::WHO_AM_I);
        if (wai != whoAmIValue)
			return false;
		
		if (burst) {
			// FIFO uses circular buffer mode, no watermark use
			writeReg(Register::F_SETUP, 0x40);
			// Make Z_LSB auto-increment to X_MSB (0x01) instead of STATUS (0x00)
			writeReg(Register::CTRL_REG3, 0x08);
		} else {
			// Clear FIFO options; we need to do this for begin() to work
			// without turning the device off and on
			writeReg(Register::F_SETUP, 0x00);
			writeReg(Register::CTRL_REG3, 0x00);
		}
		
		// Range is set by last two bits of CTRL_REG0
		writeReg(Register::CTRL_REG0, (byte) range);
		// ODR set by bits 4:2 of CTRL_REG1
		// Active mode set by bits 0:1
		writeReg(Register::CTRL_REG1, 0x3 | ((byte) odr << 2));
		return true;
    }

    /*
     * Read data from the gyroscope.
     * If FIFO is activated, then this will read the value at
     * the top of the queue; in other words, it will not be
     * very recent.
     *
     * This option is only available in active mode.
     */
	void read(sample& s) {
		byte *b = (byte *) &s;
		// Get address
		Wire.beginTransmission(i2c_addr);
		Wire.write((byte) Register::OUT_X_MSB);
		Wire.endTransmission(false);
		// Read all 6 bytes at once
		Wire.requestFrom(i2c_addr, (byte) 6);
		while (Wire.available() < 6)
			;
		// The Arduino and Gyro store data with different endianness,
		// so we need to flip the bits
		b[1] = Wire.read(); b[0] = Wire.read(); // X
		b[3] = Wire.read(); b[2] = Wire.read(); // Y
		b[5] = Wire.read(); b[4] = Wire.read(); // Z
		Wire.endTransmission(true);
    }

    /*
     * Read N (x, y, z) samples from the gyroscope into the internal
     * buffer. This should only be used when FIFO is active.
     */
	void readBurst(sample* s, size_t n) {
		byte *b = (byte *) s;
		byte in_buffer, to_read;
		
		// Get the number of samples in the buffer
		in_buffer = readReg(Register::F_STATUS) & 0x3F;
		if (n > in_buffer) n = in_buffer;
		to_read = n * 6;
		
		Wire.beginTransmission(i2c_addr);
		Wire.write((byte) Register::OUT_X_MSB);
		Wire.endTransmission(false);
		
		// requestFrom() can only get 32 bytes at once
		for (int i = 0; i < to_read; i += 32) {
			byte amt = (to_read - i);
			if (amt > 32) amt = 32;
			// Avoid sending a NACK until the very end
			Wire.requestFrom(i2c_addr, amt, false);
			while (Wire.available() < amt)
				;
			// Flip byte order from big to little endian
			for (int j = i; j < i + amt; j += 2) {
				b[j+1] = Wire.read();
				b[j]   = Wire.read();
			}
		}
		// Send NACK
		Wire.endTransmission(true);
    }

    /*
     * Convert sample to a floating-point number based on the range
     */
    float s2f(short s) {
		float fs = ((float) s) / 32768.0;
		switch (currentRange) {
			case Range::DPS_2000: return fs * 2000.0;
			case Range::DPS_1000: return fs * 1000.0;
			case Range::DPS_500:  return fs *  500.0;
			case Range::DPS_250:  return fs *  250.0;
		}
    }

	// Private function definitions
	static byte readReg(Register reg) {
		byte data;
		// Get address
		Wire.beginTransmission(i2c_addr);
		Wire.write((byte) reg);
		Wire.endTransmission(false);
		// Read data
		Wire.requestFrom(i2c_addr, (byte) 1);
		while (Wire.available() < 1)
			;
		data = Wire.read();
		Wire.endTransmission(true);
		return data;
	}

	static void writeReg(Register reg, byte val) {
		Wire.beginTransmission(i2c_addr);
		// TODO see if we could use a byte-buffer instead for one call?
		Wire.write((byte) reg);
		Wire.write(val);
		Wire.endTransmission();
	}

}
