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
namespace FXAS {
	
	/*
	 * Lines commented out with //~ are not yet implemented.
	 */
	
    struct sample {
        short x, y, z;
    };
    
    enum ODR { };
    enum RANGE { };
	
    // [TODO] Actually set this
    //~const short burst_buffer_max = -1;
	
	//Setup
	bool begin();
	//~void standbyMode();
	//~void activeMode();
	//Configuration
    //~void setODRAndRange(ODR,RANGE);
	//~void setBurst(bool);
	// Reading data
    //~void read(sample&);
    //~void read_burst(sample*, size_t);

    //Convert sample short to float in dps
    //Used for debug; we store the data as shorts
    //~float s2f(short s);
}
