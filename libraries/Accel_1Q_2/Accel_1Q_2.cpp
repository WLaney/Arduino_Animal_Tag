#include "Accel_1Q_2.h"
#include <Arduino.h>
#include <I2C.h>

namespace Accel {
	
	static byte read_single(Register);
	static void write_single(Register, byte);

	static ODR current_odr_        = ODR::HZ_12_5;
	static Range current_range_    = Range::G4;
	static FIFO_Mode current_mode_ = FIFO_Mode::DISABLED;

	bool begin(ODR odr, Range range) {
		bool result = begin_standby(odr, range);
		if  (!result) return false;
		
		set_active(true);
		return true;
	}

	bool begin_standby(ODR odr, Range range) {
		current_odr_ = odr;
		current_range_ = range;
		
		// Get WHO_AM_I
		if (read_single(Register::WHO_AM_I) != 0x1A)
			return false;
		
		// Go to standby so we can set things up
		byte r1 = read_single(Register::CTRL_REG1);
		write_single(Register::CTRL_REG1, r1 & ~0x01);
		
		// Set ODR; no low-noise or fast-read functionality
		write_single(Register::CTRL_REG1, (static_cast<byte>(odr) << 3));
		// Set Range; no high-pass filtering
		write_single(Register::XYZ_DATA_CFG, static_cast<byte>(range));
		// Set to low-power use
		write_single(Register::CTRL_REG2, 0x3);
		
		return true;
	}
	
	void set_fifo(FIFO_Mode mode) {
		current_mode_ = mode;
		write_single(Register::F_SETUP, static_cast<byte>(mode) << 6);
	}

	void set_fifo_interrupt(bool active) {
		write_single(Register::CTRL_REG4, active << 6);
		write_single(Register::CTRL_REG5, 1 << 6);
	}

	void set_active(bool active) {
		byte reg1 = read_single(Register::CTRL_REG1);
		// {bit or active, then bit and active}
		// 1 or 1 = 1 ; 1 and 1 = 1
		// 0 or 1 = 1 ; 1 and 1 = 1
		// 1 or 0 = 1 ; 1 and 0 = 0
		// 0 or 0 = 0 ; 0 and 0 = 0
		reg1 |= active;
		reg1 &= active | 0xFC;
		write_single(Register::CTRL_REG1, reg1);
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
		I2c.read(address, static_cast<byte>(Register::OUT_X_MSB), sizeof(sr), b);
		return sr;
	}
	
	/*
	 * Read the contents of the internal FIFO buffer into an n-sample
	 * array. This will only work if fifo_mode != DISABLED.
	 *
	 * If n is greater than the number of bytes in the hardware buffer,
	 * the rest of s will not be written to. The buffer cannot hold more
	 * than 32 samples, so sizes greater than n will be ignored.
	 *
	 * This returns the number of samples written.
	 */
	byte read_burst(sample_raw *s, byte n) {
		byte smps = read_single(Register::STATUS) & 0x3F;
		if (n > smps) n = smps;
		byte *b = (byte *) s;
		I2c.read(address, static_cast<byte>(Register::OUT_X_MSB), n * sizeof(sample_raw), b);
		// If interrupts are set up, we need to read F_STATUS
		// (in this case, just STATUS) to clear it
		read_single(Register::STATUS);
		return n;
	}

	byte read_burst_dsmp(sample_raw *s, byte n) {
		// For now, do this the dumb way
		sample_raw temp[2];
		byte smps;
		
		smps = read_single(Register::STATUS) & 0x3F;
		if (n*2 > smps) n = smps/2;
		
		for (byte i=0; i<n; i++) {
			I2c.read(address, static_cast<byte>(Register::OUT_X_MSB), sizeof(temp), (byte *) temp);
			s[i] = temp[0];
		}
		return n;
	}
	
	/*
	 * Convert raw samples into floating-point samples using the current
	 * accelerometer scale
	 */
	sample parse_raw(sample_raw s) {
		// Convert samples to little-endian
		// We could do this far more simply during the read_raw() step,
		// but since we likely won't use read()/parse_raw() outside of
		// debugging I think it's cool
		s.x = ((s.x & 0xFF00) >> 6) & 0x00FF | (s.x & 0x00FF) << 8;
		s.x >>= 2;
		s.y = ((s.y & 0xFF00) >> 6) & 0x00FF | (s.y & 0x00FF) << 8;
		s.y >>= 2;
		s.z = ((s.z & 0xFF00) >> 6) & 0x00FF | (s.z & 0x00FF) << 8;
		s.z >>= 2;
		
		float range;
		sample out;
		switch (current_range_) {
			case Range::G2: range = 1.0 / 4096.0; break;
			case Range::G4: range = 1.0 / 2048.0; break;
			case Range::G8: range = 1.0 / 1024.0; break;
		}
		out.x = float{s.x} * range;
		out.y = float{s.y} * range;
		out.z = float{s.z} * range;
		return out;
	}

	ODR current_odr() {
		return current_odr_;
	}

	Range current_range() {
		return current_range_;
	}
	
	static byte read_single(Register reg) {
		byte out;
		I2c.read(address, static_cast<byte>(reg), 1, &out);
		return out;
	}
	
	static void write_single(Register reg, byte val) {
		I2c.write(address, static_cast<byte>(reg), &val, 1);
	}

};
