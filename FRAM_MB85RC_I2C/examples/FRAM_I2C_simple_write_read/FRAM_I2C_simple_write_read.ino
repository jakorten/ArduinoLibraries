/**************************************************************************/
/*!
    @file     FRAM_I2C_simple_write_read.ino
    @author   SOSAndroid (E. Ha.)
    @license  BSD (see license.txt)

    Example sketch to write & read "automatic setting".

    @section  HISTORY

    v1.0.0 - First release
    V1.1.0 - Reworked code to test wider range of the memory
    v1.1.1 - Fix issue #20

    v1.2.0 - Made universal for SERCOM by J.A. Korten
    RobotPatient Simulators BV, November 5, 2021

*/
/**************************************************************************/

#include <Wire.h>
#include <FRAM_MB85RC_I2C.h>
#include "wiring_private.h" // pinPeripheral() function

#define W2_SCL 13 // PA17 D13   SERCOM1.1 SERCOM3.1
#define W2_SDA 11 // PA16 D11   SERCOM1.0 SERCOM3.0

TwoWire Wire2(&sercom1, W2_SDA, W2_SCL); // EEPROM / SRAM

byte resultw, resultr;
//dummy data used for the test
uint8_t writevalue = 0xBE;
uint8_t readvalue = 0xEF; // at the end, readvalue should be equal to writevalue


//random addresses to write from
uint16_t writeaddress = 0x025; // Beginning of the memory map


//Creating object for FRAM chip
FRAM_MB85RC_I2C mymemory(&Wire2);


void setup() {

  Serial.begin(115200);
  while (!Serial) ; //wait until Serial ready
  Wire.begin();
  Wire2.begin();

  // Assign pins 13 & 11 to SERCOM functionality
  pinPeripheral(W2_SDA, PIO_SERCOM);
  pinPeripheral(W2_SCL, PIO_SERCOM);

  Serial.println("Starting...");

  mymemory.begin();

  //--------------------------- First run, beginning of memory map ---------------------
  Serial.println("1st test");
  Serial.print("Writing at location 0x");
  Serial.println(writeaddress, HEX);

  Serial.println("Writing...");
  resultw = mymemory.writeByte(writeaddress, writevalue);
  Serial.println("Reading...");
  resultr = mymemory.readByte(writeaddress, &readvalue);

  Serial.print("Written value 0x");
  Serial.println(writevalue, HEX);

  Serial.print("Read value 0x");
  Serial.println(readvalue, HEX);

  if (writevalue == readvalue) {
    Serial.println("Write Byte test : OK");
  }
  else {
    Serial.println("Write Byte test : NOT OK");
  }
  Serial.println(".... ....");


  //------------------------- Device IDs -----------------------------------------------
  uint16_t id;
  resultr = mymemory.getOneDeviceID(1, &id);
  Serial.print("Manufacturer 0x");
  Serial.println(id, HEX);

  resultr = mymemory.getOneDeviceID(3, &id);
  Serial.print("DensityCode 0x");
  Serial.println(id, HEX);

  resultr = mymemory.getOneDeviceID(4, &id);
  Serial.print("Density ");
  Serial.println(id, DEC);


}

void loop() {
  // nothing to do
}