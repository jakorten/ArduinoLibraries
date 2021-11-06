/*

  J.A. Korten 2021
  Arduino Library for DLC_L01G sensors.

  v1.0.0 Nov 6, 2021

*/

#include "AllSensorsDLC.h"
#include "Arduino.h"

AllSensorsDLC::AllSensorsDLC(TwoWire *wire, uint8_t eocPin)
{
  this->_wire = wire;
  this->_eocPin = eocPin;
  
  pinMode(eocPin, INPUT);
  
  // for future expansion: should also work with other AllSensors DLC version.
  //this->pressure_max = pressure_max;

}

boolean AllSensorsDLC::checkForSensor()
{
  byte error;
  _wire->beginTransmission(i2c_address);
  error = _wire->endTransmission();
  return (error == 0);
}

void AllSensorsDLC::TestReadData() {
  // test method by JK
  Serial.println("We will perform a test read");

  Serial.print("I2C_ADDRESS: 0x");
  Serial.println(i2c_address, HEX);
  delay(1500);
  Serial.println("Single read operation");

  _wire->beginTransmission(i2c_address);
  int error = _wire->endTransmission();

  if (error == 0) {
    Serial.println("Device was found");
  } else {
    Serial.println("Device not found");
    return;
  }
  
  _wire->beginTransmission(i2c_address);
  _wire->write(CMD_StartSingle); // single read
  error = _wire->endTransmission();
  if (error == 0) {
    Serial.println("Command CMD_StartSingle / 0xAA was sent");
  } else {
    Serial.println("Error sending CMD_StartSingle / 0xAA");
    return;
  }
  
  _wire->beginTransmission(i2c_address);
  _wire->write(CMD_StartSingle); // single read
  //delay(100);
  while (digitalRead(this->_eocPin) == 0) {
    // ToDo: implement timeout!
  }
  
  uint8_t status = _wire->read();
  Serial.println(status);
  error = _wire->endTransmission();

  /*
  // Following receipt of one of these commands, the EOC pin is set to Low level, and the sensor Busy bit is set in the Status Byte.
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
      } * /
    delay(1000);
  }
  */
  _wire->endTransmission();
  /*
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
  */
}

bool AllSensorsDLC::readData() {
  Serial.print("Hello");
  
  _wire->beginTransmission(i2c_address);
  _wire->write(CMD_StartSingle); // single read
  _wire->endTransmission();
  
  int counter = 0;
  while (digitalRead(this->_eocPin) == 0);
  /*
  {
    counter++;
    if (counter > 10000) {
      Serial.print("Timeout in sensor");
      return false; // there must be something wrong...
    }
  }*/
  _wire->beginTransmission(i2c_address);
  _wire->requestFrom(i2c_address, (uint8_t) READ_LENGTH);
  for (int i = 0; i < READ_LENGTH; i++) {
    // check correct order:
    raw_data[i] = _wire->read();
  }
  _wire->endTransmission();

  Serial.print("Counter set to: ");
  Serial.println(counter);

  Serial.print("Read: ");
  for (int i = 0; i < READ_LENGTH; i++) {
    // check correct order:
    Serial.print(raw_data[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("Hi");

  
  /*
  status = extractStatus();
  raw_p = extractIntegerPressure();
  raw_t = extractIntegerTemperature();

  pressure = convertPressure(transferPressure(raw_p));
  temperature = convertTemperature(transferTemperature(raw_t));
  */
  //return status == Status::ERROR;
}

/*
  Pressure Output Transfer Function

  pressure_H2O = 1.25 * (PoutDig - OffsetDig / pow(2, 24)) * FullScaleSpan_InH2O

*/

/*

   Temperature Output Transfer Function

   TemperatureDegC = ((ToutDig * 150.0 / pow(2, 24)) - 40.0

*/

// EOC pin should be low when sensor is busy...

/*
  Sensor Commands: Five Measurement commands are supported,
  returning values of either a single pressure /
  temperature reading or an average of 2, 4, 8, or 16 readings.

  Each of these commands wakes the sensor from Idle state into Active state,
  and starts a measurement cycle.

  For the Start-Average commands, this cycle is repeated the appropriate numper of times,
  while the Start-Single command performs a single iteration.

  When the DSP has completed calculations and the new values have been made available to the I/O block,
  the sensor returns to Idle state.

  The sensor remains in this low-power state until another Measurement command is received.

  After completion of the measurement, the result may then be read using the Data Read command.
  The ADC and DSP remain in Idle state, and the I/O block returns the 7 bytes of status and measurement data.

  See Figure 2, following. At any time, the host may request current device status with the Status Read command.

  See Table 1 for a summary of all commands.

  For optimum sensor performance,
  All Sensors recommends that Measurement commands be issued at a fixed interval by the host system.

  Irregular request intervals may increase overall noise on the output.
*/
