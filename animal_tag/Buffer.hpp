#ifndef Buffer_H
#define Buffer_H
#include <Arduino.h>

template <class T, int size> class Buffer {
private:
	T data[size];
	size_t index;
public:
	Buffer();
	~Buffer();
	void push(T datum);
	bool isFull();
	void reset();
	// Returns a pointer to the front of the buffer
	T *get_front();
	size_t get_size();
};

#endif
