/*
 * Library for the FXAS21002C gyroscope using the alternative I2C
 * library provided by dsscircuits. It should be a bit faster when it
 * comes to burst-reading.
 */
#ifndef __FXAS_2_H__
#define __FXAS_2_H__
#include "Arduino.h"

namespace FXAS2 {
	
    struct sample {
        short x, y, z;
    };
    
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
    
	// Output data rates, in order specified by CTRL_REG1
    enum class ODR {
		HZ_800 = 0x00,
		HZ_400,
		HZ_200,
		HZ_100,
		HZ_50,
		HZ_25,
		HZ_12_5,
	};
	
	// Range, in order specified by CTRL_REG0
    enum class Range {
		DPS_2000,
		DPS_1000,
		DPS_500,
		DPS_250
	};
	
    constexpr byte bufferSize = 32;
	constexpr byte i2c_addr = 0x20;
    static_assert(bufferSize % 2 == 0, "burst_buffer_max should be even");
	
	extern Range currentRange;
	extern ODR currentOdr;
	extern bool isActive;
	
	bool begin(ODR odr, Range range, bool burst);
	bool beginSelfTest();
    void standby();
    void active();
    
    /*
     * Read and store a single sample.
     * Use readBurst when burst-mode is active.
     */
    void read(sample&);
    
    /*
     * Read up to N samples from the hardware FIFO buffer.
     * Use this when burst-mode is active.
	 * Returns the number of bytes read.
     */
    void readBurst(sample*, size_t);
	
	/*
	 * Converts one axis of the sample into degrees per second
	 * based on the current range.
	 */
    float sampleToDps(short s);
    
    /*
     * Returns the time, in milliseconds, for the chip to go from
     * standby to active mode. According to the datasheet, this is
     * (1/ODR) + 60ms.
     */
    byte timeToActive();
}

#endif //__GYRO_FXAS_H__
