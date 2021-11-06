/*

  J.A. Korten 2021
  Arduino Library for DLC_L01G sensors.

  v1.0.0 Nov 6, 2021


  https://media.digikey.com/pdf/Data%20Sheets/Amphenol%20All%20Sensors%20Corp/DLC%20DS-0365%20Rev%20A.PDF

*/

#ifndef ALLSENSORSDLC_H
#define ALLSENSORSDLC_H

#include <stdint.h>
#include <Wire.h>
#pragma once

#include "dlc_definitions.h"

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

class AllSensorsDLC {
  public:
    AllSensorsDLC(TwoWire *wire, uint8_t eocPin);
    bool checkForSensor();
    void TestReadData();
    bool readData();

  private:
    TwoWire *_wire;
    int _eocPin;
    uint8_t raw_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    static const uint8_t READ_LENGTH = 7; // see datasheet table 1
    static constexpr uint16_t FULL_SCALE_REF = (uint16_t) 1 << 14;
};

#endif // ALLSENSORSDLC_H
