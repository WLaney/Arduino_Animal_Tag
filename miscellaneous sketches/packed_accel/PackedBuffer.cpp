/*
 * PackedBuffer.cpp
 *
 *  Created on: Jul 10, 2016
 *      Author: bdpowell
 */

#include "PackedBuffer.h"
#include "Arduino.h"
#include "SD.h"

/*
 * Pack the first set of reads in. This will overwrite any
 * previous calls to pack1() or pack2(), so call it before
 * pack2().
 */
inline void packed_data::pack1(read x, read y, read z) {
	// Pack LSB into the full bytes
	this->lsb_bytes.x1 = x & 0x00FF;
	this->lsb_bytes.y1 = y & 0x00FF;
	this->lsb_bytes.z1 = z & 0x00FF;
	// Pack MSB "nibble" (four bits)
	this->msb_nibbles.x1_x2 = (x & 0x0F00) >> 4;
	this->msb_nibbles.y1_y2 = (y & 0x0F00) >> 4;
	this->msb_nibbles.z1_z2 = (z & 0x0F00) >> 4;
}

/*
 * Pack the second set of reads in. This will not overwrite
 * data from pack1(), but will be overwritten the next time
 * pack1() or pack2() is called.
 */
inline void packed_data::pack2(read x, read y, read z) {
	this->lsb_bytes.x2 = x & 0x00FF;
	this->lsb_bytes.y2 = y & 0x00FF;
	this->lsb_bytes.z2 = z & 0x00FF;

	this->msb_nibbles.x1_x2 = this->msb_nibbles.x1_x2 | (x & 0x0F00) >> 8;
	this->msb_nibbles.y1_y2 = this->msb_nibbles.y1_y2 | (y & 0x0F00) >> 8;
	this->msb_nibbles.z1_z2 = this->msb_nibbles.z1_z2 | (z & 0x0F00) >> 8;
}

inline float accel_s2f(short s, float scale) {
	return s * float{scale / (1 << 11)};
}

inline short unpack1(byte lsb, byte msb) {
	short result;
	short temp;
	result = lsb;
	result &= 0x00FF;
	temp = (msb & 0xF0) << 4;
	temp <<= 4;
	temp >>= 4;
	return result | temp;
}

inline short unpack2(byte lsb, byte msb) {
	short result;
	short temp;
	result = lsb;
	result &= 0x00FF;
	temp = (msb & 0x0F) << 8;
	temp <<= 4;
	temp >>= 4;
	return result | temp;
}

/*
 * Unpack the data and print its contents to stdout.
 * Used for testing.
 */
inline void packed_data::unpack_and_print() {
	short x1, y1, z1, x2, y2, z2;
	// 1. Turn MSB into a signed char so right shifts are arithmetic
	// 2. Right arithmetic shift keeps sign
	// 3. Add to short in such a way that it keeps sign
	x1 = unpack1(lsb_bytes.x1, msb_nibbles.x1_x2);
	y1 = unpack1(lsb_bytes.y1, msb_nibbles.y1_y2);
	z1 = unpack1(lsb_bytes.z1, msb_nibbles.z1_z2);

	Serial.print(accel_s2f(x1, 8.0F)) ; Serial.print('\t');
	Serial.print(accel_s2f(y1, 8.0F)) ; Serial.print('\t');
	Serial.println(accel_s2f(z1, 8.0F));

	// 1. Convert MSB to signed short
	// 2. Move MSB to back, then push back
	x2 = unpack2(lsb_bytes.x2, msb_nibbles.x1_x2);
	y2 = unpack2(lsb_bytes.y2, msb_nibbles.y1_y2);
	z2 = unpack2(lsb_bytes.z2, msb_nibbles.z1_z2);

	Serial.print(accel_s2f(x2, 8.0F)) ; Serial.print('\t');
	Serial.print(accel_s2f(y2, 8.0F)) ; Serial.print('\t');
	Serial.println(accel_s2f(z2, 8.0F));
}


/*
 * Sets up the buffer.
 */
template<byte packs>
PackedBuffer<packs>::PackedBuffer() {
//	buffer = new packed_data[packs];
	buffer_i = 0;
	is_read2 = false;
}

/*
 * Frees the buffer from memory. We probably won't use this...
 */
template<byte packs>
PackedBuffer<packs>::~PackedBuffer() {
//	delete[] buffer;
}

/*
 * Push a new set of reads onto the buffer. These reads must
 * be 12-bit integers; any additional bits will be discarded.
 * Returns if the push was successful (i.e. buffer is not full).
 */
template<byte packs>
bool PackedBuffer<packs>::push(read x, read y, read z) {
	if (full()) {
		return false;
	}
	packed_data &d = buffer[buffer_i];
	if (is_read2) {
		d.pack2(x,y,z);
		buffer_i++;
		is_read2 = false;
	} else {
		d.pack1(x,y,z);
		is_read2 = true;
	}

	return true;
}

/*
 * Return if the buffer is full.
 */
template<byte packs>
bool const PackedBuffer<packs>::full() {
	return buffer_i == packs;
}

/*
 * Return the total capacity of the buffer.
 */
template<byte packs>
byte const PackedBuffer<packs>::capacity() {
	return packs * 2;
}

template<byte packs>
size_t const PackedBuffer<packs>::write_size() {
  return packs * sizeof(packed_data);
}

/*
 * Print every element in the buffer.
 * Used mainly for testing.
 */
template<byte packs>
void PackedBuffer<packs>::write_all(File sd) {
	sd.write((byte *) buffer, packs * sizeof(packed_data));
}

/*
 * Reset the buffer so that it can be written to
 * again. This does not actually clear the data in
 * the buffer.
 */
template<byte packs>
void PackedBuffer<packs>::reset() {
	buffer_i = 0;
	is_read2 = false;
}

/*
 * Print every element in the buffer.
 * Used mainly for testing.
 */
template<byte packs>
void PackedBuffer<packs>::print_all() {
	for (byte i = 0; i < packs; i++) {
		buffer[i].unpack_and_print();
	}
}


/*
 * Template Specializations
 */
// accel.cpp
template class PackedBuffer<24>;
