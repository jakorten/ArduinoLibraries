// --------------------------------------
// i2c_scanner for SAMDxx microcontrollers with Arduino bootloader 
// (e.g. Feather M0 Express by Adafruit)
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//
// Changes for RobotPatient Simulators and HAN University of Applied Sciences
// Nov 5, 2021 J.A. Korten
// Scanner for 3 i2c channels:
// Escpecially for DevBoard for Ventilations / Soft Robotics actuators.
//
// Based on earlier work by Krodal and Nick Gammon
// See also: https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-wire
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//
//
// Changes for RobotPatient Simulators and HAN University of Applied Sciences
// Nov 5, 2021 J.A. Korten
// Scanner for 3 i2c channels:
// Escpecially for DevBoard for Ventilations / Soft Robotics actuators.
//

#include <Wire.h> // SERCOM3?
#include "wiring_private.h" // pinPeripheral() function

// i2c system bus
#define W0_SCL 21 // PA22 D20 / SDA SERCOM3.0 SERCOM5.0
#define W0_SDA 20 // PA23 D21 / SCL SERCOM3.1 SERCOM5.1

#define W1_SCL 3 // PA09  D3    SERCOM0.1 SERCOM2.1
#define W1_SDA 4 // PA08  D4    SERCOM0.0 SERCOM2.0

#define W2_SCL 13 // PA17 D13   SERCOM1.1 SERCOM3.1
#define W2_SDA 11 // PA16 D11   SERCOM1.0 SERCOM3.0

// SDA will be on SERCOM1.0 D11 and SCL will be on SERCOM1.1 D13

// Default Wire:
// I2C / SERCOM 0:
// PA22 / SDA on pin 11;
// PA23 / SCL on pin 12;

// On the Development Board:
// D13_SCL_PA17 /﻿D11_SDA_PA16
// D3_SCL_PA09 / D4_SDA_PA08
// SCL_PA23 / SDA_PA22 are they mixed up? Needs checking Nov 5, 2021. SFM Could be switched using jumper wires on the pads JP9/JP10

TwoWire Wire0(&sercom3, W0_SDA, W0_SCL); // Right sensor
TwoWire Wire1(&sercom2, W1_SDA, W1_SCL); // Left sensor, diff sensor
TwoWire Wire2(&sercom1, W2_SDA, W2_SCL); // EEPROM / SRAM
// And of course standard Wire

void setup()
{
  Serial.begin(115200);
  delay(1500);
  Wire.begin();
  Wire0.begin();
  Wire1.begin();
  Wire2.begin();

  // Assign pins 4 & 3 to SERCOM functionality
  pinPeripheral(W1_SDA, PIO_SERCOM_ALT);
  pinPeripheral(W1_SCL, PIO_SERCOM_ALT);

  // Assign pins 13 & 11 to SERCOM functionality
  pinPeripheral(W2_SDA, PIO_SERCOM);
  pinPeripheral(W2_SCL, PIO_SERCOM);
  delay(1500);

  Serial.println("Ready...");
  delay(1500);
}


void loop()
{
  //exit;
  digitalWrite(12, HIGH);
  reportDevicesOn(&Wire0, "0: on SERCOM3 (D20d/D21c)");
  reportDevicesOn(&Wire1, "1: on SERCOM2 (D4_SDA_PA08 / ﻿D3_SCL_PA09)");
  reportDevicesOn(&Wire2, "2: on SERCOM1 (D11_PA16_SDA1 /﻿D13_PA17_SCL1)");

  delay(2500);           // wait 5 seconds for next scan
}

void reportDevicesOn(TwoWire *wire, String label) {
  byte error, address;
  int nDevices;
  Serial.print("Scanning bus 1: ");
  Serial.println(label);
  for (address = 1; address < 127; address++ )
  {
    digitalWrite(12, LOW);
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    wire->beginTransmission(address);
    error = wire->endTransmission();

    if (error == 0)
    {
      digitalWrite(12, HIGH);
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      printDeviceName(address);

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found on bus 1\n");
  else
    Serial.println("done\n");
}

void printDeviceName(int address) {
  // Print recognized i2c device names:
  if (address == 0x25) {
    Serial.println("Sensirion SDP800-500Pa found...");
  }
  if (address == 0x26) {
    Serial.println("Sensirion SDP800-501Pa found...");
  }
  if (address == 0x29) {
    Serial.println("DLC-L01G-U2 found...");
  }
  if (address == 0x40) {
    Serial.println("Sensirion SDP610-500Pa found...");
  }
  if (address == 0x50) {
    Serial.println("FRAM/EEPROM found...");
  }
  if (address == 0x51) {
    Serial.println("More memory found? Could be 1M FRAM.");
  }
  if (address == 0x7C) {
    Serial.println("RESERVED...");
  }
}
