/**************************************************************************/
/*!
    @file     FRAM_I2C_connect.ino
    @author   SOSAndroid (E. Ha.)
    @license  BSD (see license.txt)

    This sketch connects the FRAM chips and tries to identify it. This succeed if the IDs are known, fails otherwise

    @section  HISTORY

    v1.0.0 - First release

    v1.1.0 - Made universal for SERCOM by J.A. Korten
    RobotPatient Simulators BV, November 5, 2021
*/
/**************************************************************************/

#include <Wire.h>
#include <math.h>

#include <FRAM_MB85RC_I2C.h>
#include "wiring_private.h" // pinPeripheral() function


#define W2_SCL 13 // PA17 D13   SERCOM1.1 SERCOM3.1
#define W2_SDA 11 // PA16 D11   SERCOM1.0 SERCOM3.0

TwoWire Wire2(&sercom1, W2_SDA, W2_SCL); // EEPROM / SRAM

//Creating object for FRAM chip
FRAM_MB85RC_I2C mymemory(&Wire2); // = FRAM_MB85RC_I2C(void);

void setup() {

  Serial.begin(9600);
  while (!Serial) ; //wait until Serial ready
  Wire.begin();
  Wire2.begin();

  // Assign pins 13 & 11 to SERCOM functionality
  pinPeripheral(W2_SDA, PIO_SERCOM);
  pinPeripheral(W2_SCL, PIO_SERCOM);

  Serial.println("Starting...");

  mymemory.begin();
  //While SERIAL_DEBUG enabled at the library level, this should output the device's IDs


}

void loop() {
  // nothing to do
}