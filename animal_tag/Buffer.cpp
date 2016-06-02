#include "Buffer.hpp"
#include "debug.h"

template <typename value_type, int size>
Buffer<value_type, size>::Buffer() {
	this->index = 0;
}

template <typename value_type, int size>
void Buffer<value_type, size>::push(value_type datum) {
#ifdef DEBUG
	if (isFull()) {
		DBGLN("ERROR: Tried to push on a full buffer");
		return;
	}
#endif
	data[index++] = datum;
}

template <typename value_type, int size>
bool Buffer<value_type, size>::isFull() {
	return index == size;
}

template <typename value_type, int size>
void Buffer<value_type, size>::reset() {
	index = 0;
}

template <typename value_type, int size>
value_type *Buffer<value_type, size>::get_front() {
	return data;
}

template <typename value_type, int size>
size_t Buffer<value_type, size>::get_size() {
	return size;
}
