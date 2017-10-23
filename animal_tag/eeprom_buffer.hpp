#include <EEPROM.h>

/*
 * Fixed-size circular buffer within EEPROM.
 */
template <class T>
class rom_buffer {
private:
	int start_address;
	int max_size;
	int current_index;

	bool is_initialized(int index) {
		int addr = get_addr(index);
		for (int i=0; i<sizeof(T); i++) {
			int v = EEPROM.read(addr + i);
			if (v != 0xFF)
				return true;
		}
		return false;
	}

	int get_addr(int index) {
		return start_address + index * sizeof(T);
	}
public:
	rom_buffer(int addr, int size) {
		start_address = addr;
		max_size = size;

    current_index = 0;
	}
	
	bool push(T value) {
		int addr = get_addr(current_index);
		EEPROM.put(addr, value);
		current_index = (current_index + 1) % max_size;
	}

	T *read_all() {
    T v;
		T *arr = new T[max_size];
		for (int i=0; i<max_size; i++) {
      EEPROM.get(get_addr(i), v);
			arr[i] = v;
		}
		return arr;
	}
	
};
