
#include "fxas_2.h"
#include "Arduino.h"
#include "I2C.h"
#include <stddef.h>

namespace FXAS2 {
	
	inline static byte readReg(Register reg) {
		byte out;
		I2c.read(i2c_addr, static_cast<byte>(reg), 1, &out);
		return out;
	}
	
	inline static void writeReg(Register reg, byte data) {
		I2c.write(i2c_addr, static_cast<byte>(reg), &data, 1);
	}
	
	// Value factory-coded into WHO_AM_I register
	constexpr byte whoAmIValue = 0xD7;
	
	Range currentRange = Range::DPS_250;
	ODR currentOdr = ODR::HZ_12_5;
	bool isActive = false;
	
    /*
     * Start the gyroscope in standby mode.
     * This returns whether or not the gyro could be successfully reached.
     */
	bool begin(ODR odr, Range range, bool burst) {
		isActive = true; // well, it'll be true after this function returns
		currentRange = range;
		currentOdr = odr;
		I2c.begin();
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
		writeReg(Register::CTRL_REG1, 0x2 | ((byte) odr << 2));
		return true;
    }
    
    bool beginSelfTest() {
		I2c.begin();
		// Go into standby mode
		writeReg(Register::CTRL_REG1, 0x00);
		// Check WHO_AM_I byte
		byte wai = readReg(Register::WHO_AM_I);
        if (wai != whoAmIValue)
			return false;
		
		writeReg(Register::CTRL_REG1, 1 << 5);
		return true;
	}
	
	/*
	 * Go to standby mode. The device can be configured in this mode,
	 * but isn't collecting data. It also saves power.
	 */
	void standby() {
		isActive = false;
		byte ctrl_reg1 = readReg(Register::CTRL_REG1);
		ctrl_reg1 &= ~0x3; // clear active and ready bits
		writeReg(Register::CTRL_REG1, ctrl_reg1);
	}
	
	/*
	 * Go to active mode. The device is collecting data, but can't
	 * be configured.
	 */
	void active() {
		isActive = true;
		byte ctrl_reg1 = readReg(Register::CTRL_REG1);
		ctrl_reg1 &= ~0x3; // need to go to standby before setting mode?
		ctrl_reg1 |= 0x2;  // set active bit
		writeReg(Register::CTRL_REG1, ctrl_reg1);
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
		I2c.read(i2c_addr, static_cast<byte>(Register::OUT_X_MSB), 6, b);
    }

    /*
     * Read N (x, y, z) samples from the gyroscope into the internal
     * buffer. This should only be used when FIFO is active.
     */
	void readBurst(sample* s, size_t n) {
		byte *sb = (byte *) s;
		byte nb = n * sizeof(sample);
		I2c.read(i2c_addr, static_cast<byte>(Register::OUT_X_MSB), nb, sb);
    }

    /*
     * Convert sample to a floating-point number based on the range
     */
    float sampleToDps(short s) {
		float fs = ((float) s) / 32768.0;
		switch (currentRange) {
			case Range::DPS_2000: return fs * 2000.0;
			case Range::DPS_1000: return fs * 1000.0;
			case Range::DPS_500:  return fs *  500.0;
			case Range::DPS_250:  return fs *  250.0;
		}
    }
    
    /*
     * Return time, in ms, to get from standby to active mode.
     * Calculated from datasheet: (1/ODR) + 60ms
     */
    byte timeToActive() {
		switch (currentOdr) {
			case ODR::HZ_800:  return byte{(1000.0 / 800.0) + 60.0}; //61
			case ODR::HZ_400:  return byte{(1000.0 / 400.0) + 60.0};
			case ODR::HZ_200:  return byte{(1000.0 / 200.0) + 60.0};
			case ODR::HZ_100:  return byte{(1000.0 / 100.0) + 60.0};
			case ODR::HZ_50:   return byte{(1000.0 /  50.0) + 60.0};
			case ODR::HZ_25:   return byte{(1000.0 /  25.0) + 60.0};
			case ODR::HZ_12_5: return byte{(1000.0 /  12.5) + 60.0}; //140
		}
	}

}
