
// Use this in your ino/cpp sketches to turn on profiling.
//#define USE_PROFILER

namespace Profiler {
	void begin();      // Begin profiling, create bins, etc.
	void setBin(byte); // Send time spent to this bin
	void dumpBins();   // Print the time spent in each bin and clear
	void clear();      // Reset bin data
}
