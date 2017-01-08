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
#include "Arduino.h"

namespace FXAS {
	
	/*
	 * Lines commented out with //~ are not yet implemented.
	 */
	
    struct sample {
        short x, y, z;
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
	
    // Actual maximum is 31 or 32
    constexpr byte burst_buffer_max = 10;
	constexpr byte i2c_addr = 0x20;
	
	bool begin(ODR odr, Range range, bool burst);
    void read(sample&);
    void readBurst(sample*, size_t);

    //Convert sample short to float in dps
    //Used for debug; we store the data as shorts
    float s2f(short s);
}

#endif //__GYRO_FXAS_H__
