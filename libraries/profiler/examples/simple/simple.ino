#include <profiler.h>
#define USE_PROFILER

using P = Profiler;

/*
 * Bin directory:
 *  1 - General processing
 *  2 - big_cpu_fn_1
 *  3 - big_cpu_fn_2
 *  7 - delay cycles
 */

void setup() {
	P::begin();
	P::setBin(0);
}

void update() {
	P::setBin(1);
	big_cpu_fn_1(); // Goes to bin 2
	big_cpu_fn_2(); // Goes to bin 3
	P::delay(20);   // Goes to bin 7
	
	P::setBin(1);
	P::dumpBins(); // Print bin information and clear
}
