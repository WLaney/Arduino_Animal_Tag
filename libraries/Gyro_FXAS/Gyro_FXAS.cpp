#include "Gyro_FXAS.hpp"
#include "stddef.h"

namespace FXAS {
	
    /*
     * Start the gyroscope in standby mode.
     * This returns whether or not the gyro could be successfully reached.
     */
	bool begin() {
		// [TODO] Implement
        return false;
    }

    /*
     * Put the gyro in standby mode.
     * The gyro cannot read data, but its options can be changed.
     */
	void standbyMode() {

    }

    /*
     * Put the gyro in active mode.
     * Its options cannot be changed, but it can read data.
     */
	void activeMode() {

    }

    /*
     * Set the output data rate and range of the gyroscope.
     * The two values are contained in the same register, so you
     * have to set them at the same time.
     *
     * This option is only available during standby mode.
     */
    void setODRAndRange(ODR,RANGE) {

    }

    /*
     * Turn burst-reading on or off.
     * The gyroscope has an internal FIFO buffer that it can store
     * data in.
     *
     * This option is only available during standby mode.
     */
	void setBurst(bool) {

    }

    /*
     * Read data from the gyroscope.
     * If FIFO is activated, then this will read the value at
     * the top of the queue; in other words, it will not be
     * very recent.
     *
     * This option is only available in active mode.
     */
	void read(sample&) {

    }

    /*
     * Read N (x, y, z) samples from the gyroscope into the internal
     * buffer. This should only be used when FIFO is active.
     * 
     * The size of the FIFO buffer is defined in the documentation.
     * [TODO] Figure that out
     */
	void read_burst(sample*, size_t) {

    }

    /*
     *
     */
    float s2f(short s) {

    }

}
