/*

  This software is licensed under the Revised (3-clause) BSD license as follows:
  Copyright (c) 2018, Jeremy Cole <jeremy@jcole.us>

  All rights reserved.

  See the LICENSE file for more details.

  J.A. Korten 2021: modifications for DLC versions

*/

#include "AllSensors_DLC.h"
#include "Arduino.h"

enum States {busy, ready}; // linked to Status, but in this case separate this state is indicated by the eocPin as well (not just based on status reading)

AllSensors_DLC::AllSensors_DLC(TwoWire *bus, uint8_t eocPin, SensorType type, float pressure_max) :
  pressure_unit(PressureUnit::PSI),
  temperature_unit(TemperatureUnit::CELCIUS)
{
  this->bus = bus;
  this->type = type;
  this->pressure_max = pressure_max;
  this->eocPin = eocPin;

  switch (type) {
    case GAGE:
    case ABSOLUTE:
      pressure_zero_ref = 1638;
      pressure_range = pressure_max;
      break;
    case DIFFERENTIAL:
      pressure_zero_ref = 8192;
      pressure_range = pressure_max * 2;
      break;
  }
}

bool AllSensors_DLC::checkForSensor() {
  // checks for sensor at given address
  /*
      pre-requisite:
      Wire should be active...
  */

  this->bus->beginTransmission(I2C_ADDRESS);
  int error = this->bus->endTransmission();

  return (error == 0);
}

void AllSensors_DLC::TestReadData() {
  // test method by JK
  Serial.println("We will perform a test read");

  Serial.print("I2C_ADDRESS: 0x");
  Serial.println(I2C_ADDRESS, HEX);
  delay(1500);
  Serial.println("Single read operation");

  bus->beginTransmission(I2C_ADDRESS);
  int error = bus->endTransmission();

  if (error == 0) {
    Serial.println("Device was found");
  } else {
    Serial.println("Device not found");
    return;
  }
  
  bus->beginTransmission(I2C_ADDRESS);
  bus->write(0xAA); // single read
  error = bus->endTransmission();
  if (error == 0) {
    Serial.println("Command 0xAA was sent");
  } else {
    Serial.println("Error sending 0xAA");
    return;
  }
  while (digitalRead(this->eocPin) == 0) { //wait till EOC becomes high

    //Serial.println("BUS request");
    //bus->requestFrom(I2C_ADDRESS, (uint8_t) READ_LENGTH);
    uint8_t status = bus->read();

    bus->write(0xAA);
    Serial.println(status, BIN);
    /*

      for (int i = 0; i < READ_LENGTH; i++) {
      raw_data[i] = bus->read();

      Serial.print(i);
      Serial.print(" = ");
      Serial.print(raw_data[i]);
      Serial.println();
      } */
    delay(1000);
  }

  bus->endTransmission();

  status = extractStatus();
  raw_p = extractIntegerPressure();
  raw_t = extractIntegerTemperature();

  Serial.print("status: ");
  Serial.println(status);


  pressure = convertPressure(transferPressure(raw_p));
  temperature = convertTemperature(transferTemperature(raw_t));

  Serial.print("pressure: ");
  Serial.println(pressure);

  Serial.print("temperature: ");
  Serial.println(temperature);


  //return status == Status::ERROR;

}

bool AllSensors_DLC::readData() {
  bus->requestFrom(I2C_ADDRESS, (uint8_t) READ_LENGTH);

  for (int i = 0; i < READ_LENGTH; i++) {
    // check correct order:
    raw_data[i] = bus->read();
  }

  bus->endTransmission();

  status = extractStatus();
  raw_p = extractIntegerPressure();
  raw_t = extractIntegerTemperature();

  pressure = convertPressure(transferPressure(raw_p));
  temperature = convertTemperature(transferTemperature(raw_t));

  return status == Status::ERROR;
}
