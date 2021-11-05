/**************************************************************************/
/*!
    @file     FRAM_I2C_readIDs.ino
    @author   SOSAndroid (E. Ha.)
    @license  BSD (see license.txt)

    This sketch tries to read the devices IDs without the help of the library. For debugging purpose

    @section  HISTORY

    v1.0.0 - First release

    v1.1.0 - Made universal for SERCOM by J.A. Korten    
    RobotPatient Simulators BV, November 5, 2021

*/
/**************************************************************************/
#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function

#define W2_SCL 13 // PA17 D13   SERCOM1.1 SERCOM3.1
#define W2_SDA 11 // PA16 D11   SERCOM1.0 SERCOM3.0

uint8_t i2c_addr = 0x50;
uint16_t manufacturer, densitycode, productid;
uint8_t mb85rc_slaveid = 0xF8;

TwoWire Wire2(&sercom1, W2_SDA, W2_SCL); // EEPROM / SRAM

void setup() {

  Serial.begin(115200);
  while (!Serial) ; //wait until Serial ready
  Wire.begin();
  Wire2.begin();

  // Assign pins 13 & 11 to SERCOM functionality
  pinPeripheral(W2_SDA, PIO_SERCOM);
  pinPeripheral(W2_SCL, PIO_SERCOM);

  Serial.println("Starting...");
  Serial.println("....... .......");

  getDeviceIDs();
  deviceIDs2Serial();

}

void loop() {
  // nothing to do
}

void getDeviceIDs(TwoWire *wire)
{
  uint8_t localbuffer[3] = { 0, 0, 0 };
  uint8_t result;

  /* Get device IDs sequence 	*/
  /* 1/ Send 0xF8 to the I2C bus as a write instruction. bit 0: 0 => 0xF8 >> 1 */
  /* 2/ Write the device address on the bus */
  /* 3/ Request 3 bytes at 0xF9, read bit: 1 => 0xF8>>1 */

  wire->beginTransmission(mb85rc_slaveid >> 1);
  wire->write((byte)(i2c_addr << 1));
  result = wire->endTransmission(false);

  wire->requestFrom(mb85rc_slaveid >> 1, 3);
  localbuffer[0] = (uint8_t) wire->read();
  localbuffer[1] = (uint8_t) wire->read();
  localbuffer[2] = (uint8_t) wire->read();

  /* Shift values to separate IDs */
  manufacturer = (localbuffer[0] << 4) + (localbuffer[1] >> 4);
  densitycode = (uint16_t)(localbuffer[1] & 0x0F);
  productid = ((localbuffer[1] & 0x0F) << 8) + localbuffer[2];
}

void deviceIDs2Serial(void) {

  if (Serial) {
    Serial.println("FRAM Device IDs");
    Serial.print("Manufacturer 0x"); Serial.println(manufacturer, HEX);
    Serial.print("ProductID 0x"); Serial.println(productid, HEX);
    Serial.print("Density code 0x"); Serial.println(densitycode, HEX);
    Serial.println("...... ...... ......");
  }
}