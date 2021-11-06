/*

  J.A. Korten 2021
  Arduino Library for DLC_L01G sensors.

  v1.0.0 Nov 6, 2021

*/

// DLC Sensor Command Set 
#define CMD_StartSingle 0xAA
#define CMD_StartAverage2 0xAC
#define CMD_StartAverage4 0xAD
#define CMD_StartAverage8 0xAE
#define CMD_StartAverage16 0xAF

// Sensor Address
#define i2c_address 0x29
