//DO NOT FORGET TO SET DATA/TIME. LINE 32

#include <Wire.h> 
#include <SFE_MMA8452Q.h> 
#include <SD.h>
#include <SPI.h>
#include <DS3234.h>

int tmp102Address = 0x48;
const int chipSelect = 10; //microSD 

MMA8452Q accel;


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  accel.init(SCALE_4G, ODR_12);
 //intialize RTC:
  RTC.configure(11,12,13,9);
//RTC.setDateTime(int d, int mo, int y, int h, int mi, int s);
RTC.setDateTime(9,2,16,10,30,00);

 SPI.setDataMode(SPI_MODE0); // switch mode to SD
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  // intialize card:
 SD.begin(chipSelect);
  //intialize I2C:
  Wire.begin();
 
  //may need to swtich SPI mode here
  Serial.print("working");

}

void loop() {
  
 read_write();
 delay(1000);
}

//=====================================
void read_write(void) {
    // strings for assembling the data to log:
  String acx = "";
  String acy = "";
  String acz = "";
            
   // read the three directions in Gs and append to the strings:
   accel.read();
    float X_accel = accel.cx;
    acx += String(X_accel);
    float Y_accel = accel.cy;
    acy += String(Y_accel);
    float Z_accel = accel.cz;
    acz += String(Z_accel);
    
  
  //read temp sensor in celsius
  Wire.requestFrom(tmp102Address,2); 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  float celsius = TemperatureSum*0.0625;
  
   
   //read RTC:
  //SPI.setDataMode(SPI_MODE3);  // switch mode to clock
  String time = RTC.readDateTime();
   
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //SPI.setDataMode(SPI_MODE0);  // switch mode to SD
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(time);
    dataFile.print("\t");
    dataFile.print(acx);
    dataFile.print("\t");
    dataFile.print(acy);
    dataFile.print("\t");
    dataFile.print(acz);
    dataFile.print("\t");
    dataFile.print(celsius);
    dataFile.print("\t");
    dataFile.println("\t");
    dataFile.close();
     // SPI.setDataMode(SPI_MODE3);  // switch mode to clock
   
  }
  Serial.print("done");
  //SPI.endTransaction();
}

