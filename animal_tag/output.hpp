/*
 * Functions for writing data to the SD card from the sensors. 
 */

// Setup the SD card. Returns if successful.
bool output_setup();
// Write some configuration data to the SD card. Returns if successful.
bool output_write_header();
// Write data to the SD card. If use_long is true, then long-term data is also written.
void output_write_data(bool use_long);
