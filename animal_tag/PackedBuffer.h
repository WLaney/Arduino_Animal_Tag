/*
 * PackedBuffer.h
 *
 * A buffer that packs 2 sensor reads, each consisting of 3 12-bit
 * signed integers, into 9 bytes. This technique trades CPU for RAM,
 * which is vital with our Arduino pro mini/micro.
 * 
 *  Created on: Jul 10, 2016
 *      Author: bdpowell
 */

#ifndef PACKEDBUFFER_H_
#define PACKEDBUFFER_H_
#include "Arduino.h"
#include "SD.h"

//using byte = uint8_t;
using read = uint16_t;

/*
 * Packs two sensor reads into 9 bytes. Has an alignment of 1 byte,
 * preventing data loss due to structure alignment.
 */
struct packed_data {
	// We store the full LSBs of each read in here
	struct {
		byte x1, y1, z1;
		byte x2, y2, z2;
	} lsb_bytes;
	// Then we store each "nibble" (the top 4 bits)
	// In each case, read1 takes the most significant nibble, and
	// read2 the least
	struct {
		byte x1_x2;
		byte y1_y2;
		byte z1_z2;
	} msb_nibbles;

	// TL;DR: Call pack1, then pack2 to prevent overwriting data
	void pack1(read x, read y, read z);
	void pack2(read x, read y, read z);
	// Print both reads to Serial; will be replaced later
	void unpack_and_print();
};

/*
 * A buffer to hold N "packs" of sensor reads efficiently. Encapsulates
 * reads somewhat, which is obviously a good thing.
 */
template<byte packs>
class PackedBuffer {
private:
	packed_data *buffer;
	byte buffer_i;
	bool is_read2;
public:
	PackedBuffer();
	virtual ~PackedBuffer();
	bool push(read x, read y, read z);
	bool full();
	byte capacity();
	void write_all(File sd);
	void reset();
};

#endif /* PACKEDBUFFER_H_ */
