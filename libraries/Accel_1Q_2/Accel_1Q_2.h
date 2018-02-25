#ifndef __ACCEL_1Q_H__
#define __ACCEL_1Q_H__
#include <Arduino.h>

namespace Accel {

	enum class Register {
		STATUS = 0x00,
		OUT_X_MSB = 0x01,
		OUT_X_LSB = 0x02,
		OUT_Y_MSB = 0x03,
		OUT_Y_LSB = 0x04,
		OUT_Z_MSB = 0x05,
		OUT_Z_LSB = 0x06,

		F_SETUP = 0x09,
		TRIG_CFG = 0x0A,
		SYSMOD = 0x0B,
		INT_SOURCE = 0x0C,
		WHO_AM_I = 0x0D,
		XYZ_DATA_CFG = 0x0E,
		HP_FILTER_CUTOFF = 0x0F,
		PL_STATUS = 0x10,
		PL_CFG = 0x11,
		PL_COUNT = 0x12,
		PL_BF_ZCOMP = 0x13,
		P_L_THS_REG = 0x14,
		FF_MT_CFG = 0x15,
		FF_MT_SRC = 0x16,
		FF_MT_THS = 0x17,
		FF_MT_COUNT = 0x18,

		TRANSIENT_CFG = 0x1D,
		TRANSIENT_SCR = 0x1E,
		TRANSIENT_THS = 0x1F,
		TRANSIENT_COUNT = 0x20,
		PULSE_CFG = 0x21,
		PULSE_SRC = 0x22,
		PULSE_THSX = 0x23,
		PULSE_THSY = 0x24,
		PULSE_THSZ = 0x25,
		PULSE_TMLT = 0x26,
		PULSE_LTCY = 0x27,
		PULSE_WIND = 0x28,
		ASLP_COUNT = 0x29,
		CTRL_REG1 = 0x2A,
		CTRL_REG2 = 0x2B,
		CTRL_REG3 = 0x2C,
		CTRL_REG4 = 0x2D,
		CTRL_REG5 = 0x2E,
		OFF_X = 0x2F,
		OFF_Y = 0x30,
		OFF_Z = 0x31
	};
	
	enum class ODR {
		HZ_800  = 0x0, // Lower rates use less power, are smoother
		HZ_400  = 0x1,
		HZ_200  = 0x2,
		HZ_100  = 0x3,
		HZ_50   = 0x4,
		HZ_12_5 = 0x5,
		HZ_6_25 = 0x6,
		HZ_1_56 = 0x7
	};

	enum class Range {
		G2 = 0x0, // That's 2g's
		G4 = 0x1,
		G8 = 0x2
	};

	enum class FIFO_Mode {
		DISABLED = 0b00, // Disable the internal FIFO buffer
		CIRCULAR = 0b01, // Circular buffer; new samples replace oldest ones
		FILL     = 0b10  // Stop collecting new samples if the buffer is full
		//TRIGGER -- Not implemented
	};
	
	constexpr byte address = 0x1d; // SA0 is high -> 0x1C
	constexpr byte buffer_size = 32;
	
	/*
	 * Compactly stores 3 14-bit big-endian integer samples. They're
	 * stored (in binary) as:
	 * MMMMMMMMLLLLLL00
	 *
	 * This means that they're not really usable outside of storage.
	 * Convert them via parse_raw(), or just use read().
	 */
	struct sample_raw {
		short x, y, z;
	};

	/*
	 * Stores 3 floating-point samples, corrected for accelerometer range.
	 */
	struct sample {
		float x, y, z;
	};

	/*
	 * Start the tag without using any advanced features.
	 * The tag will immediately start collecting data.
	 */
	bool begin(ODR, Range);

	/*
	 * Start the tag in standby mode.
	 * It will not collect data, but advanced features (like set_fifo,
	 * set_fifo_interrupt) can be set before making the tag active
	 * (set_active).
	 */
	bool begin_standby(ODR, Range);

	/*
	 * Configure the tag's internal FIFO buffer.
	 * This will fail if the tag is in active mode.
	 */
	void set_fifo(FIFO_Mode);

	/*
	 * Configure the tag's FIFO interrupt capability.
	 * 
	 * If this is set to true, then the tag will assert an interrupt
	 * from the INT1 pin whenever the FIFO overflows. This should be
	 * detected via FALLING-edge processing (I think...?)
	 */
	void set_fifo_interrupt(bool);

	/*
	 * Return the number of samples in the FIFO buffer
	 */
	int fifo_n_samples();

	/*
	 * Put the tag into active or standby mode.
	 * While active, it can receive data, but advanced features cannot
	 * be used.
	 */
	void set_active(bool);
	
	/*
	 * Read a raw sample. Faster and leaner than read(), but the output
	 * can't be used.
	 */
	sample_raw read_raw();
	
	/*
	 * Read a sample and convert it to floating-point.
	 */
	sample read();
	
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
	byte read_burst(sample_raw *s, byte n);
	
	/*
	 * Burst-read (n*2) samples from the accelerometer and downsample
	 * by a factor of 2. This is mostly useful for simulating a 25Hz
	 * sample rate.
	 */
	byte read_burst_dsmp(sample_raw *s, byte n);
	
	/*
	 * Convert a raw sample into a floating-point one based on the
	 * current accelerometer range. Storing a read in sample_raw uses
	 * less memory, but they can't be used arithmetically.
	 */
	sample parse_raw(sample_raw);
	
	/*
	 * Get the current Output Data Rate.
	 */
	ODR current_odr();
	
	/*
	 * Get the current range.
	 */
	Range current_range();
};

#endif //__ACCEL_1Q_H__
