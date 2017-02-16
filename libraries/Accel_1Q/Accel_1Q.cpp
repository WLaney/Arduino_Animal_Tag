#include "Accel_1Q.h"
#include <Wire.h>

namespace Accel {
	
	bool begin(ODR odr, Range range) {
		current_odr_ = odr;
		current_range_ = range;
		/*
		 * WHO_AM_I
		 * XYZ_DATA_CFG
		 * CTRL_REG1
		 * CTRL_REG2
		 */
		
		// Go to standby so we can set things up
		byte r1 = read_single(Register::CTRL_REG1);
		
		// Get WHO_AM_I
		if (read_single(Register::WHO_AM_I) != 0x1A)
			return false;
		
		write_single(Register::CTRL_REG1, r1 & ~0x01);
		// Set Range; no high-pass filtering
		write_single(Register::XYZ_DATA_CFG, range);
		// Set to low-power use
		write_single(Register::CTRL_REG2, 0x2);
		// Set ODR; no low-noise or fast-read functionality
		// Then go into active mode!
		write_single(Register::CTRL_REG1, (odr << 3) | 0x1);
		
		return true;
	}

	sample read() {
		return parse_raw(read_raw());
	}
	
	sample_raw read_raw() {
		
	}
	
	sample parse_raw(sample_raw) {
		
	}

	ODR current_odr() {
		return current_odr_;
	}

	Range current_range() {
		return current_range_;
	}
	
	static byte read_single(Register reg) {
		Wire.beginTransmission(address);
		Wire.write(byte{reg});
		Wire.endTransmission(address, false);
		Wire.requestFrom(address, 1, true);
		return Wire.read();
	}
	
	static void write_single(Register reg, byte val) {
		Wire.beginTransmission(address);
		Wire.write(byte{reg});
		Wire.write(val);
		Wire.endTransmission();
	}

};
