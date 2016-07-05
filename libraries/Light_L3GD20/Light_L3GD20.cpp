/***************************************************
  This is a library for the L3GD20 and L3GD20H GYROSCOPE

  Designed specifically to work with the Adafruit L3GD20(H) Breakout 
  ----> https://www.adafruit.com/products/1032

  These sensors use I2C or SPI to communicate, 2 pins (I2C) 
  or 4 pins (SPI) are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Kevin "KTOWN" Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Light_L3GD20.h>
#include "Wire.h"
#include "Arduino.h"

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
namespace Gyro {
  
  static const byte address = L3GD20_ADDRESS;
  static const l3gd20Range_t range = L3DS20_RANGE_250DPS;
    
  byte read8(l3gd20Registers_t reg);
  void write8(l3gd20Registers_t reg, byte value);

  bool begin()
  {
    Wire.begin();

    /* Make sure we have the correct chip ID since this checks
     for correct address and that the IC is properly connected */
    uint8_t id = read8(L3GD20_REGISTER_WHO_AM_I);
    if ((id != L3GD20_ID) && (id != L3GD20H_ID))
    {
      return false;
    }

    /* Set CTRL_REG1 (0x20)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7-6  DR1/0     Output data rate                                   00
     5-4  BW1/0     Bandwidth selection                                00
     3  PD        0 = Power-down mode, 1 = normal/sleep mode          1
     2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
     1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
     0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */

    /* Switch to normal mode and enable all three channels */
    write8(L3GD20_REGISTER_CTRL_REG1, 0x0F);
    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG2 (0x21)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     5-4  HPM1/0    High-pass filter mode selection                    00
     3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */

    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG3 (0x22)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
     0  I2_Empty  FIFO empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */

    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG4 (0x23)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
     6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
     5-4  FS1/0     Full scale selection                               00
                    00 = 250 dps
                    01 = 500 dps
                    10 = 2000 dps
                    11 = 2000 dps
     0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */

    /* Adjust resolution if requested */
    switch(range)
    {
    case L3DS20_RANGE_250DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x00);
      break;
    case L3DS20_RANGE_500DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x10);
      break;
    case L3DS20_RANGE_2000DPS:
      write8(L3GD20_REGISTER_CTRL_REG4, 0x20);
      break;
    }
    /* ------------------------------------------------------------------ */

    /* Set LOW_ODR (0x39)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7    N/A 
     6    N/A 
     5    DRDY_HL   DRDY/INT2 pin active level                          0
     4    0         Keep this at 0!                                     0
     3    I2C_dis   Disable I2C if set to 1.                            0
     2    SW_RES    Software reset.                                     0
     1    0         Keep this at 0!                                     0
     0    Low_ODR   Low speed ODR; 1 to enable.                         0*/

    // We want Low_ODR enabled!
    write8(L3GD20_REGISTER_LOW_ODR, 0x01);
    /* ------------------------------------------------------------------ */
    
#ifdef L3GD20_USE_FIFO
    /* Set CTRL_REG5 (0x24)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  BOOT      Reboot memory content (0=normal, 1=reboot)          0
     6  FIFO_EN   FIFO enable (0=FIFO disable, 1=enable)              0
     4  HPen      High-pass filter enable (0=disable,1=enable)        0
         5  StopOnFTH Limit FIFO depth to FIFO_CTRL FTH4's limit          0
     3-2  INT1_SEL  INT1 Selection config                              00
     1-0  OUT_SEL   Out selection config                               00 */

    write8(L3GD20_REGISTER_CTRL_REG5, 0x40); // Enable FIFO
    /* ------------------------------------------------------------------ */

    /* Set FIFO_CTRL (0x2E)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
         7-5  FM2:0     FIFO mode selection.                              000
         0-4  FTH4:0    FIFO threshold setting.                             0*/

      /*
         * FM2 FM1 FM0 FIFO mode
         * --- --- --- ---------
         *   0   0   0  Bypass
         *   0   0   1  FIFO; reading stops when buffer full
         *   0   1   0  Stream; oldest values discarded
         *   Look up the rest on the datasheet...
         */
        
        write8(L3GD20_REGISTER_FIFO_CTRL_REG, 0x40); // FIFO stream mode on
    /* ------------------------------------------------------------------ */
#endif

    return true;
  }

  /***************************************************************************
   PUBLIC FUNCTIONS
   ***************************************************************************/

  /*
   * Read a single XYZ value from the gyroscope into *d.
   * If FIFO is enabled, this will pop the oldest value in the FIFO queue.
   * For more efficient FIFO reading, though, it is reccomended that you use
   * burst_read instead.
   */
  void read(l3gd20Data_t *d)
  { 
    Wire.beginTransmission(address);
    // Make sure to set address auto-increment bit
    Wire.write(L3GD20_REGISTER_OUT_X_L | 0x80);
    Wire.endTransmission();
    Wire.requestFrom(address, (byte)6);
    
    // Wait around until enough data is available
    while (Wire.available() < 6);
    
    // Kind of sick, but direct
    char *c = (char *) d;
    c[0] = Wire.read();
    c[1] = Wire.read();
    c[2] = Wire.read();
    c[3] = Wire.read();
    c[4] = Wire.read();
    c[5] = Wire.read();
  }
  
  // Convert a raw gyroscope read into a float
  float s2f(short s) {
    switch (range) {
      case L3DS20_RANGE_250DPS:
        return s * L3GD20_SENSITIVITY_250DPS;
        break;
      case L3DS20_RANGE_500DPS:
        return s * L3GD20_SENSITIVITY_500DPS;
        break;
      case L3DS20_RANGE_2000DPS:
        return s * L3GD20_SENSITIVITY_2000DPS;
        break;
    }
  }

#ifdef L3GD20_USE_FIFO
  const byte max_wire = 32;
  /*
   * Pull at most max sensor reads into the ds array.
   * Returns the number of reads written, and doesn't try
   * to pull more reads than the sensor has.
   */
  byte fifo_burst_read(l3gd20Data_t *ds, byte max) {
    byte fl = fifo_get_length();
	byte mn = (fl < max) ? fl : max;
	byte to_read = mn * 6;

    Wire.beginTransmission(address);
    // Make sure to set address auto-increment bit
    Wire.write(L3GD20_REGISTER_OUT_X_L | 0x80);
    Wire.endTransmission();
    
    byte *b = (byte *) ds;
    byte left = (to_read / max_wire) * max_wire;
    for (byte i=0; i<left; i+=max_wire) {
		Wire.requestFrom(address, max_wire-1, false);
		while (Wire.available() < max_wire-1)
			;
		// get reads
		for (byte j=i; j<i+max_wire; j++) {
			b[j] = Wire.read();
		}
	}
	
	// Leftovers
	Wire.requestFrom(address, to_read-left, true);
	while (Wire.available() < to_read-left)
		;
	for (byte i=left; i<to_read; i++) {
		b[i] = Wire.read();
	}

    return mn;
  }

  /*
   * Get the number of elements currently in the FIFO buffer.
   * If FIFO is disabled, this returns 1, since, well, that's how
   * bypass mode works.
   */
  byte fifo_get_length(void) {
    byte src_reg = read8(L3GD20_REGISTER_FIFO_SRC_REG);
    return src_reg & 0x1F;
  }
#endif /*USE_FIFO */

  /***************************************************************************
   PRIVATE FUNCTIONS
   ***************************************************************************/
  void write8(l3gd20Registers_t reg, byte value)
  {
    Wire.beginTransmission(address);
    Wire.write((byte)reg);
    Wire.write(value);
    Wire.endTransmission();
  }

  byte read8(l3gd20Registers_t reg)
  {
    byte value;

    Wire.beginTransmission(address);
    Wire.write((byte)reg);
    Wire.endTransmission();
    Wire.requestFrom(address, (byte)1);
    value = Wire.read();
    Wire.endTransmission();

    return value;
  }


}
