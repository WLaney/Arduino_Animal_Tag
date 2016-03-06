//DO NOT FORGET TO SET DATA/TIME

//include libraries
//=====================================
#include <Wire.h> 
#include <SFE_MMA8452Q.h> 
#include <SPI.h>
#include "ds3234.h"
#include <SD.h>

//initialise SD card
//=====================================
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10; // chip select pin for SD

#define BUFF_MAX 256

//initialise RTC
//=====================================
const int cs = 9; // chip select pin for RTC

uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 5000; //Test if I nead this, I think no

//initialis accelormater
//=====================================
MMA8452Q accel;


//initialis temp sensor
//=====================================
int tmp102Address = 0x48;

void setup()
{
  // Open serial communications and wait for port to open:
   Serial.begin(9600);

 //begin RTC  
 //=====================================
 DS3234_init(cs, DS3234_INTCN);
 memset(recv, 0, BUFF_MAX - 1);

//begin SD card  
 //===================================== 
  SPI.setDataMode(SPI_MODE0); // switch mode to SD
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  // intialize card:
 SD.begin(chipSelect);
 
 //begin I2c  
 //=====================================
  Wire.begin();

 //begin accel  
 //=====================================
  accel.init(SCALE_8G, ODR_6);
 
//confrim that the everything is working and there is serial communication
  Serial.print("working");

}

void loop() {

//Read accelerometer 
 //=====================================
            
   // read the three directions in Gs and append to the strings:
   accel.read();
    float X_accel = accel.cx;
    float Y_accel = accel.cy;
    float Z_accel = accel.cz;

    //print accel to serial with float values
    Serial.println(" ");
    Serial.print(X_accel);
    Serial.print("\t");
    Serial.print(Y_accel);
    Serial.print("\t");
    Serial.print(Z_accel);
    Serial.print("\t");

 //read temp sensor in celsius
 //=====================================
  Wire.requestFrom(tmp102Address,2); 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  float celsius = TemperatureSum*0.0625;
  Serial.print(celsius); Serial.print("\t");//print temp to serial

//Read RTC  
 //=====================================
   SPI.setDataMode(SPI_MODE1); //switch to RTC mode
  char buff[BUFF_MAX];
    unsigned long now = millis();
    int in;
struct ts t;
    // show time
    DS3234_get(cs, &t);
//       snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
//             t.mon, t.mday, t.hour, t.min, t.sec);
//        Serial.print(buff); //print time to serial
Serial.print(t.sec); //print second to serial to confirm time is working


 //Write data to SD
 //=====================================       
 SPI.setDataMode(SPI_MODE0);  // switch mode to SD
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    //data writen as tab seprated values in order of
    //time,  accel in x,  accel in y,  accel in z,  temp
    dataFile.print("working"); //test writing to SD
    dataFile.print("\t");
    dataFile.print(t.year);
    dataFile.print("-");
    dataFile.print(t.mon);
    dataFile.print("-");
    dataFile.print(t.mday);
    dataFile.print("\t");
    dataFile.print(t.hour);
    dataFile.print(":");
    dataFile.print(t.min);
    dataFile.print(":");
    dataFile.print(t.sec);
    dataFile.print("\t");
    dataFile.print(X_accel);
    dataFile.print("\t");
    dataFile.print(Y_accel);
    dataFile.print("\t");
    dataFile.print(Z_accel);
    dataFile.print("\t");
    dataFile.print(celsius);
    dataFile.print("\t");
    dataFile.close(); //need to close the data file or can not wirte new data
   Serial.println("done"); //confirm data has been writen
  }

//wait 100ms before begingin the loop again
delay(100);
 
}
