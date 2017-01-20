/*
 * FXAS20021c gyroscope controller.
 * Made specifically for our Animal Tag project; supports I2c and
 * FIFO reading.
 * 
 * This library starts the gyroscope in standby mode after begin() is
 * called, at which point you can configure its range, output rate,
 * etc. through various function calls. Use activeMode() to end
 * configuration and start collecting data, and use read() or
 * read_burst() to get data (depending on if you chose to use
 * FIFO mode).
 * 
 * Samples are stored as short signed 12-bit ints; their conversion
 * to dps is dependent on the range of the gyroscope. Use s2f to convert
 * shorts to floats.
 */
#ifndef __GYRO_FXAS_H__
#define __GYRO_FXAS_H__
#include <stddef.h>
#include <assert.h>
#include "Arduino.h"

namespace FXAS {
	
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
    static_assert(bufferSize % 2 == 0, "burst_buffer_max should be even");
	constexpr byte i2c_addr = 0x20;
	
	extern Range currentRange;
	
	bool begin(ODR odr, Range range, bool burst);
    void read(sample&);
    void readBurst(sample*, size_t);
	
    //Convert sample short to float in dps
    //Used for debug; we store the data as shorts
    float s2f(short s);
}

#endif //__GYRO_FXAS_H__
