// Quick-and-dirty gyro
// i2c only
// hardcoded bandwidth and data rate
#include <Arduino.h>

class FXAS_Gyro {
public:

  struct fxas_data {
    float x, y, z;
  };

  enum registers {
    WHO_AM_I = 0x0c,
    CTRL_REG0 = 0x0d, // bandwidth and range
    CTRL_REG1 = 0x03 // output data rate
  };

  bool begin();
  void read();

  fxas_data data;

private:

  // Can either be 0x20 or 0x21, depending
  // on how William builds it
  const static byte address = 0x21;
  const static byte who_am_i_value = 0x7D;
  
  byte read8(registers reg);
  void write8(byte reg, byte value);
};

