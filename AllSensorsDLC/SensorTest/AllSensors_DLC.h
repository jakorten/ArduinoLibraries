/*

Support for the AllSensors DLV Series Low Voltage Digital Pressure Sensors

See the following datasheet:

https://www.allsensors.com/datasheets/DS-0336_Rev_E.pdf

The sensors are sold in a few varieties, with the following options:
  * Pressure range options from 5 to 60 PSI.
  * Type options of differential, gage, or absolute.

For simplicity, pre-configured subclasses of AllSensors_DLC which align with
the purchaseable model numbers are sensors can be found at the bottom of this
file.

* * *

This software is licensed under the Revised (3-clause) BSD license as follows:

Copyright (c) 2018, Jeremy Cole <jeremy@jcole.us>

All rights reserved.

See the LICENSE file for more details.

J.A. Korten 2021: modifications for DLC versions

https://media.digikey.com/pdf/Data%20Sheets/Amphenol%20All%20Sensors%20Corp/DLC%20DS-0365%20Rev%20A.PDF

*/

#ifndef ALLSENSORS_DLC_H
#define ALLSENSORS_DLC_H

#include <stdint.h>
#include <Wire.h>
#pragma once

/* From datasheet DLC-L01G-U2
 *  Pressure(in H2O) = 1.24 * (Pout_dig - OSdig / 2^24) * FSS(inH2O)
 *  where OSdig is specified digital offset output from  Performance Characteristics Table
 *  where The sensor Full Scale Span in inches H2O
 *   - For Gage Operating Range sensors: Full Scale Pressure
 *   - For Differential Operating Range sensors: 2 x Full Scale Pressure.
 *  Tempc = (Tout_dig * 150/2^24) - 40
 *  
 *  Sensor status request" 1-byte read
 *  Sensor data request 7-byte read
 *  
 *  Measure: single read is 0xAA
 *  Average 2x 0xAC, 4x 0xAD, 8x 0xAE, 16x 0xAF
 *  
 *  S[7:0] P[23:16] P[15:8] P[7:0] T[23:16] T[15:8] T[7:0]
 *  status pressure 3xByte / temperature 3xByte 
 *  
 *  Status bits
 *  ==========================================
 *  7 Always 0
 *  6 Power On, 1
 *  5 Busy 1, processing, 0, ready
 *  4:3 Mode 00 Normal operation
 *  2 Memory error 1 is eeprom checksum fail
 *  1 sensor configuration (always 0)
 *  0 ALU error (1 = Error)
 */

class AllSensors_DLC {
public:

  // The default I2C address from the datasheet.
  static const uint8_t I2C_ADDRESS = 0x29;
  
  // The sensor type, where part numbers:
  //   * DLC-xxxG-* are GAGE sensors.
  //   * DLC-xxxD-* are DIFFERENTIAL sensors.
  //   * DLC-xxxA-* are ABSOLUTE sensors.
  //  JK Note: as I only use the DLC_L01G sensor, I did not test the other sensors (Jan, 2021)
  enum SensorType {
    GAGE         = 'G',
    DIFFERENTIAL = 'D',
    ABSOLUTE     = 'A',
  };
  
  enum Status {
    OK          = 0,
    BUSY        = 1,
    NOPOWER     = 6,
    ERROR       = 7,
    MEM_ERROR   = 8,
    ALU_ERROR   = 9
  };

  enum PressureUnit {
    PSI    = 'L',
    IN_H2O = 'H',
    PASCAL = 'P',
  };

  enum TemperatureUnit {
    CELCIUS    = 'C',
    FAHRENHEIT = 'F',
    KELVIN     = 'K',
  };

  bool checkForSensor();

private:

  static const uint8_t READ_LENGTH = 7; // see datasheet table 1
  
  static constexpr uint16_t FULL_SCALE_REF = (uint16_t) 1 << 14;

  
  TwoWire *bus;
  SensorType type;
  uint8_t eocPin;

  uint8_t raw_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  
  uint16_t raw_p = 0;
  uint16_t raw_t = 0;

  float pressure_max;
  float pressure_range;
  float pressure_zero_ref;

  PressureUnit pressure_unit;
  TemperatureUnit temperature_unit;

  // Extract the 2-bit status field, bits 0-1.
  Status extractStatus() {
    #define SB 0 //  ==  STATUS BYTE position
    //return (Status)((raw_data[0] & 0b11000000) >> 6);

    uint8_t power     = (raw_data[SB] & 0b01000000) >> 6;
    uint8_t busy      = (raw_data[SB] & 0b00100000) >> 5;
    uint8_t mode      = (raw_data[SB] & 0b00011000) >> 3;
    uint8_t mem_state = (raw_data[SB] & 0b00000100) >> 2;
    uint8_t alu_error = (raw_data[SB] & 0b00000001);

    if (power == 1) {
      if (busy == 1) {
        return BUSY;
      } else {
        if (mem_state == 1) {
          return MEM_ERROR; // EEPROM Checksum fail
        }
        if (alu_error == 1) {
          return ALU_ERROR; // ALU Error
        }
        if (mode == 0) {
          return OK; // normal operation
        }
      }
    } else {
      return NOPOWER; // is this possible?
    }

    return ERROR; // something went wrong
  }

  // Extract the 24-bit pressure field, byte [3:5].
  uint32_t extractIntegerPressure() {
    return ((uint32_t)(raw_data[1] << 16) | (uint32_t)(raw_data[2] << 8) | (uint32_t)(raw_data[3]));
  }
  
  // Extract the 24-bit temperature field, byte [0:2].
  uint16_t extractIntegerTemperature() {
    return ((uint32_t)(raw_data[4] << 16) | (uint32_t)(raw_data[5] << 8) | (uint32_t)(raw_data[6]));
  }
  
  // Convert a raw digital pressure read from the sensor to a floating point value in PSI.
  float transferPressure(unsigned long raw_value) {
    // Based on the following formula in the datasheet:
    //     Pressure(psi) = 1.25 x ((P_out_dig - OS_dig) / 2^14) x FSS(psi)
    return 1.25 * (((float)raw_value - pressure_zero_ref) / FULL_SCALE_REF) * pressure_range;
  }
  
  // Convert a raw digital temperature read from the sensor to a floating point value in Celcius.
  float transferTemperature(unsigned long raw_value) {
    // Based on the following formula in the datasheet:
    //     Temperature(degC) = T_out_dig x (200 / 2^11 - 1) - 50
    return (float)raw_value * (200.0 / 2047.0) - 50.0;
  }

  // Convert the input in PSI to the configured pressure output unit.
  float convertPressure(float psi) {
    switch(pressure_unit) {
      case PASCAL:
        return psi * 6894.75729;
      case IN_H2O:
        return psi * 27.679904;
      case PSI:
      default:
        return psi;
    }
  }

  // Convert the input in Celcius to the configured temperature output unit.
  float convertTemperature(float degree_c) {
    switch(temperature_unit) {
      case FAHRENHEIT:
        return degree_c * 1.8 + 32.0;
      case KELVIN:
        return degree_c + 273.15;
      case CELCIUS:
      default:
        return degree_c;
    }    
  }

public:

  Status status;
  float pressure;
  float temperature;

  AllSensors_DLC(TwoWire *bus, uint8_t eocPin, SensorType type, float pressure_max);
  void TestReadData();


  // Set the configured pressure unit for data output (the default is inH2O).
  void setPressureUnit(PressureUnit pressure_unit) {
    this->pressure_unit = pressure_unit;
  }

  // Set the configured temperature unit for data output (the default is Celcius).
  void setTemperatureUnit(TemperatureUnit temperature_unit) {
    this->temperature_unit = temperature_unit;
  }

  bool readData();
};

// We only tested DLC-L01G-U2

class AllSensors_DLC_L01G : public AllSensors_DLC {
public:
  AllSensors_DLC_L01G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 1.0) {}
};

class AllSensors_DLC_L02G : public AllSensors_DLC {
public:
  AllSensors_DLC_L02G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 2.0) {}
};

class AllSensors_DLC_L05G : public AllSensors_DLC {
public:
  AllSensors_DLC_L05G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 5.0) {}
};


class AllSensors_DLC_L10G : public AllSensors_DLC {
public:
  AllSensors_DLC_L10G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 10.0) {}
};

class AllSensors_DLC_L20G : public AllSensors_DLC {
public:
  AllSensors_DLC_L20G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 20.0) {}
};

class AllSensors_DLC_L30G : public AllSensors_DLC {
public:
  AllSensors_DLC_L30G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 30.0) {}
};

class AllSensors_DLC_L60G : public AllSensors_DLC {
public:
  AllSensors_DLC_L60G(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::GAGE, 60.0) {}
};

// Differential versions:

class AllSensors_DLC_L01D : public AllSensors_DLC {
public:
  AllSensors_DLC_L01D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 1.0) {}
};

class AllSensors_DLC_L02D : public AllSensors_DLC {
public:
  AllSensors_DLC_L02D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 2.0) {}
};

class AllSensors_DLC_L05D : public AllSensors_DLC {
public:
  AllSensors_DLC_L05D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 5.0) {}
};


class AllSensors_DLC_L10D : public AllSensors_DLC {
public:
  AllSensors_DLC_L10D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 10.0) {}
};

class AllSensors_DLC_L20D : public AllSensors_DLC {
public:
  AllSensors_DLC_L20D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 20.0) {}
};

class AllSensors_DLC_L30D : public AllSensors_DLC {
public:
  AllSensors_DLC_L30D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 30.0) {}
};

class AllSensors_DLC_L60D : public AllSensors_DLC {
public:
  AllSensors_DLC_L60D(TwoWire *bus, uint8_t eocPin) : AllSensors_DLC(bus, eocPin, AllSensors_DLC::SensorType::DIFFERENTIAL, 60.0) {}
};

// ToDo: add High pressure variants (see datasheet) page 3.


/*
class AllSensors_DLC_005D : public AllSensors_DLC {
public:
  AllSensors_DLC_005D(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::DIFFERENTIAL, 5.0) {}
};

class AllSensors_DLC_015D : public AllSensors_DLC {
public:
  AllSensors_DLC_015D(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::DIFFERENTIAL, 15.0) {}
};

class AllSensors_DLC_030D : public AllSensors_DLC {
public:
  AllSensors_DLC_030D(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::DIFFERENTIAL, 30.0) {}
};

class AllSensors_DLC_060D : public AllSensors_DLC {
public:
  AllSensors_DLC_060D(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::DIFFERENTIAL, 60.0) {}
};

class AllSensors_DLC_005G : public AllSensors_DLC {
public:
  AllSensors_DLC_005G(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::GAGE, 5.0) {}
};

class AllSensors_DLC_015G : public AllSensors_DLC {
public:
  AllSensors_DLC_015G(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::GAGE, 15.0) {}
};

class AllSensors_DLC_030G : public AllSensors_DLC {
public:
  AllSensors_DLC_030G(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::GAGE, 30.0) {}
};

class AllSensors_DLC_060G : public AllSensors_DLC {
public:
  AllSensors_DLC_060G(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::GAGE, 60.0) {}
};

class AllSensors_DLC_015A : public AllSensors_DLC {
public:
  AllSensors_DLC_015A(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::ABSOLUTE, 15.0) {}
};

class AllSensors_DLC_030A : public AllSensors_DLC {
public:
  AllSensors_DLC_030A(TwoWire *bus) : AllSensors_DLC(bus, AllSensors_DLC::SensorType::ABSOLUTE, 30.0) {}
};
*/
#endif // ALLSENSORS_DLC_H
