#include "Accel_1Q.h"
#include <Arduino.h>
#include <Wire.h>

namespace Accel {
	
	static byte read_single(Register);
	static void write_single(Register, byte);

	static ODR current_odr_ = ODR::HZ_12_5;
	static Range current_range_ = Range::G4;
	
	bool begin(ODR odr, Range range) {
		current_odr_ = odr;
		current_range_ = range;
		
		// Go to standby so we can set things up
		byte r1 = read_single(Register::CTRL_REG1);
		write_single(Register::CTRL_REG1, r1 & ~0x01);
		
		// Get WHO_AM_I
		if (read_single(Register::WHO_AM_I) != 0x1A)
			return false;
		
		// Set Range; no high-pass filtering
		write_single(Register::XYZ_DATA_CFG, static_cast<byte>(range));
		// Set to low-power use
		write_single(Register::CTRL_REG2, 0x2);
		// Set ODR; no low-noise or fast-read functionality
		// Then go into active mode!
		write_single(Register::CTRL_REG1, (static_cast<byte>(odr) << 3) | 0x1);
		
		return true;
	}

	sample read() {
		return parse_raw(read_raw());
	}
	
	/*
	 * Read and return a single sample from the accelerometer.
	 */
	sample_raw read_raw() {
		sample_raw sr;
		byte *b = (byte *) &sr;
		Wire.beginTransmission(address);
		Wire.write(static_cast<byte>(Register::OUT_X_MSB));
		Wire.endTransmission(false);
		Wire.requestFrom(address, byte{6}, byte{true});
		for (byte i=0; i<6; i++) {
			b[i] = Wire.read();
		}
	}
	
	sample parse_raw(sample_raw s) {
		// Assume 14-bit; the accelerometer can do 8-bit, but we don't
		// support that functionality
		// Flip to little-endian
		s.x = ((s.x & 0x00FF) << 8) | ((s.x & 0xFF00) >> 8);
		s.y = ((s.y & 0x00FF) << 8) | ((s.y & 0xFF00) >> 8);
		s.z = ((s.z & 0x00FF) << 8) | ((s.z & 0xFF00) >> 8);
		// Convert by range
		float range;
		sample out;
		switch (current_range_) {
			case Range::G2: range = 1.0 / 2.0; break;
			case Range::G4: range = 1.0 / 4.0; break;
			case Range::G8: range = 1.0 / 8.0; break;
		}
		out.x = float{out.x} / float{1 << 14} * range;
		out.y = float{out.y} / float{1 << 14} * range;
		out.z = float{out.z} / float{1 << 14} * range;
	}

	ODR current_odr() {
		return current_odr_;
	}

	Range current_range() {
		return current_range_;
	}
	
	static byte read_single(Register reg) {
		Wire.beginTransmission(address);
		Wire.write(static_cast<byte>(reg));
		Wire.endTransmission(false);
		Wire.requestFrom(address, byte{1}, byte{true});
		return Wire.read();
	}
	
	static void write_single(Register reg, byte val) {
		Wire.beginTransmission(address);
		Wire.write(static_cast<byte>(reg));
		Wire.write(val);
		Wire.endTransmission();
	}

};
