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

//const int buff_max = 256;

//initialise RTC
//=====================================
const int cs = 9; // chip select pin for RTC

uint8_t time[8];
// char recv[buff_max];
// unsigned int recv_size = 0;
//unsigned long prev, interval = 5000; //Test if I nead this, I think no

//initialize accelerometer
//=====================================
MMA8452Q accel;


//initialis temp sensor
//=====================================
int tmp102Address = 0x48;

void setup()
{
   // Open serial communications and wait for port to open:
   Serial.begin(9600);

   // Begin RTC
   DS3234_init(cs, DS3234_INTCN);
   //memset(recv, 0, buff_max - 1);

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

  // Write the datetime that this was opened
  //Read RTC  
  //=====================================
  SPI.setDataMode(SPI_MODE1); //switch to RTC mode
  struct ts t;
  DS3234_get(cs, &t);
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(":");
  Serial.println(t.sec);
  SPI.setDataMode(SPI_MODE0);
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(t.mon);
    dataFile.print("/");
    dataFile.print(t.mday);
    dataFile.print("/");
    dataFile.print(t.year);
    dataFile.print("\t");
    dataFile.print(t.hour);
    dataFile.print(":");
    dataFile.print(t.min);
    dataFile.print(":");
    dataFile.print(t.sec);
    dataFile.println("");
    dataFile.close(); //need to close the data file or can not wirte new data

    Serial.println("Wrote date to SD");
  }
 
  //confrim that the everything is working and there is serial communication
  Serial.println("\n====\nworking\n====");

}

void loop() {
  //data writen as tab seprated values in order of
  //time (month\day\year Hr:min:sec), temp, accel in x,  accel in y,  accel in z
  //print time data
  read_to_buffer();
  //wait 80ms (approx 12Hz) before begingin the loop again
  delay(80); 
}

// Struct for accelerometer data.
struct accel_data {
  float x, y, z;
};

// Our buffer will take up to N bytes of memory.
const int buff_max_length = 256 / sizeof(accel_data);
accel_data buff[buff_max_length];
size_t buff_length = 0;

// Read accelerometer data and push it to the buffer.
// This will stop execution to flush TO the buffer if necessary...
void read_to_buffer()
{
  // If we're out of space, write to the SD card
  if (buff_length == buff_max_length) {
    flush_and_write();
  } else {
    // Get the value that we're working on
    accel_data *value = &buff[buff_length];
    accel.read();
    value->x = accel.cx;
    value->y = accel.cy;
    value->z = accel.cz;
    
    Serial.print(value->x);
    Serial.print("\t");
    Serial.print(value->y);
    Serial.print("\t");
    Serial.print(value->z);
    Serial.println("");
    
    buff_length++;
  }
}

// Flush the buffer to the SD card, writing temperature and time
// data as you do.
void flush_and_write()
{
  // +--NOTE--+
  // We need to make sure that we can tell that data X was collected between
  // times Y and Z!
  //read temp sensor in celsius
  //=====================================
  Wire.requestFrom(tmp102Address,2); 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  float celsius = TemperatureSum * 0.0625;
  // Serial prints
  Serial.println("=====FLUSHING=====");
  Serial.print(celsius); 
  Serial.print("\t");
  
  //Read RTC  
  //=====================================
  SPI.setDataMode(SPI_MODE1); //switch to RTC mode
  //char buff[buff_max];
  //unsigned long now = millis();
  // int in;
  struct ts t;
  // show time
  DS3234_get(cs, &t);
  Serial.print(t.hour); //print to serial to confirm time is working
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(":");
  Serial.println(t.sec);
  SPI.setDataMode(SPI_MODE0);
  
  //Write data to SD
  //=====================================       
  SPI.setDataMode(SPI_MODE0);  // switch mode to SD
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {

    // accel
    for (accel_data &d : buff) {
      dataFile.print(d.x);
      dataFile.print("\t");
      dataFile.print(d.y);
      dataFile.print("\t");
      dataFile.println(d.z);
    }
    
    dataFile.print(t.mon);
    dataFile.print("/");
    dataFile.print(t.mday);
    dataFile.print("/");
    dataFile.print(t.year);
    dataFile.print("\t");
    dataFile.print(t.hour);
    dataFile.print(":");
    dataFile.print(t.min);
    dataFile.print(":");
    dataFile.print(t.sec);
    dataFile.print("\t");
    
    //print temp data
    dataFile.print(celsius);
    dataFile.println("");
      
    //write data and close
    dataFile.close(); //need to close the data file or can not wirte new data
    
    Serial.println("done"); //confirm data has been writen
  }
  // Reset the buffer to the beginning
  buff_length = 0;
}
