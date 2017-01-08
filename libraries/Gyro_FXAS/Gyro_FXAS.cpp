#include "Gyro_FXAS.h"
#include "Arduino.h"
#include "Wire.h"
#include <stddef.h>

namespace FXAS {
	
	// Register addresses
    enum class Register {
		STATUS = 0x00,
		OUT_X_MSB,
		OUT_X_LSB,
		OUT_Y_MSB,
		OUT_Y_LSB,
		OUT_Z_MSB,
		OUT_Z_LSB,
		DR_STATUS,
		F_STATUS,
		F_SETUP,
		F_EVENT,
		INT_SRC_FLAG,
		WHO_AM_I,
		CTRL_REG0,
		RT_CFG,
		RT_SRC,
		RT_THS,
		RT_COUNT,
		TEMP,
		CTRL_REG1,
		CTRL_REG2,
		CTRL_REG3
	};
	
	byte readReg(Register);
	void writeReg(Register, byte);
	
	// Value factory-coded into WHO_AM_I register
	constexpr byte whoAmIValue = 0xD7;
	
    /*
     * Start the gyroscope in standby mode.
     * This returns whether or not the gyro could be successfully reached.
     */
	bool begin(ODR odr, Range range, bool burst) {
		// TODO add FIFO
		// Once we get into FIFO, we should look into resetting the device
		Wire.begin();
		// Go into standby mode
		writeReg(Register::CTRL_REG1, 0x00);
		// Check WHO_AM_I byte
		byte wai = readReg(Register::WHO_AM_I);
        if (wai != whoAmIValue)
			return false;

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

    }

    /*
     * Read N (x, y, z) samples from the gyroscope into the internal
     * buffer. This should only be used when FIFO is active.
     * 
     * The size of the FIFO buffer is defined in the documentation.
     * [TODO] Figure that out
     */
	void read_burst(sample* s, size_t n) {

    }

    /*
     *
     */
    float s2f(short s) {

    }

	// Private function definitions

	byte readReg(Register reg) {
		byte data;
		// Get address
		Wire.beginTransmission(i2c_addr);
		Wire.write((byte) reg);
		Wire.endTransmission(false);
		// Read data
		Serial.println(F("Read data"));
		Wire.requestFrom(i2c_addr, 1);
		while (Wire.available() < 1)
			;
		data = Wire.read();
		Wire.endTransmission(true);
		return data;
	}

	void writeReg(Register reg, byte val) {
		Wire.beginTransmission(i2c_addr);
		// TODO see if we could use a byte-buffer instead for one call?
		Wire.write((byte) reg);
		Wire.write(val);
		Wire.endTransmission();
}

}
