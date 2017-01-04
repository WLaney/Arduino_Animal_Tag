FXAS21002C-Arduino [![Build Status](https://travis-ci.org/sabas1080/FXAS21002C_Arduino_Library.svg?branch=master)](https://travis-ci.org/sabas1080/FXAS21002C_Arduino_Library)
================== 

Arduino library for the FXAS21002C 3-Axis Digital Angular Rate Gyroscope
# Install

## Recommended
1. Download the github repo
2. Extract the file into your Arduino's library folder
3. Include & Enjoy!

## Command-line
* Navigate to the Arduino library folder
* Clone this github repo into the FXAS21002C folder
* Include code into your project and enjoy!

# Usage
To be continued...

# Issues

## FSR/ODR Not Properly Set On Initialization
**How I Know:** Wrote a test script in `Examples/test-thorough` to check FSR/ODR after initialization.
**Explanations:**
 * It takes a certain amount of time for the chip to go from active to standby, so the FSR/ODR setting instructions
   are effectively skipped. *To check:* Put a delay in between standby() and writeReg()
 * There are issues in readReg(), writeReg(), standby(), and active() that cause multiple issues.
   *To check:* Read the datasheet to check the code against the standard.
 * The FSR and ODR are reset whenever we go into standby, which would invalidate our test.
   *To check:* Set the FSR to 250DPS and swing it around, trying to hit the ceiling.

## Reset doesn't work
**How I Know:** initSelfTest() uses the built-in reset(), which causes the device to hang.
**Explanations:**
 * The while loop is hanging because the on-boot bit isn't going to one. *To check:* put Serial message in while
   loop.
 * The while loop is polling for the on-boot bit so frequently that it breaks the chip. *To check:* put delay in between
   polls.
 * readReg()'s extra endTransmission() is causing the while loop to corrupt the chip somehow.
   *To check:* remove the endTransmission()
 * writeReg() or something

## Self-Test Doesn't Work
**How I Know:** I put in initSelfTest(), which set the self-test bit to 1 during standby.
**Explanations:**
 * I didn't read the datasheet and don't know what I'm doing.

Original Library by Andres Sabas Agosto 2015
Additions and Bugfixes by Ben Powell 2016-17
For BuildNight Instructables of The Inventor's House and Freescale and the Sharkduino project
