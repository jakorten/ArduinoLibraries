/*

    Test version for ALL Sensors

*/

#include <Wire.h>
#include "AllSensorsDLC.h"
#include "wiring_private.h" // pinPeripheral() function

// i2c system bus
#define W0_SCL 21 // PA22 D20 / SDA SERCOM3.0 SERCOM5.0
#define W0_SDA 20 // PA23 D21 / SCL SERCOM3.1 SERCOM5.1

#define W1_SCL 3 // PA09  D3    SERCOM0.1 SERCOM2.1
#define W1_SDA 4 // PA08  D4    SERCOM0.0 SERCOM2.0
#define EOC_B  17

#define W2_SCL 13 // PA17 D13   SERCOM1.1 SERCOM3.1
#define W2_SDA 11 // PA16 D11   SERCOM1.0 SERCOM3.0
#define EOC_A  16

TwoWire Wire1(&sercom2, W1_SDA, W1_SCL); //
TwoWire Wire2(&sercom1, W2_SDA, W2_SCL); //

AllSensorsDLC sensorA = AllSensorsDLC(&Wire2, EOC_A);
AllSensorsDLC sensorB = AllSensorsDLC(&Wire1, EOC_B);

boolean foundSensor = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  while (!Serial);
  setupSensors();
}

void loop() {
  if (not foundSensor) {
    Serial.println("Alas... restart");
    return;
  }
  // put your main code here, to run repeatedly:
  //sensorA.TestReadData();
  
  Serial.print(" EOC A: ");
  Serial.print(digitalRead(EOC_A));
  Serial.print(" EOC B: ");
  Serial.println(digitalRead(EOC_B));
  delay(100);
  Serial.println("Reading sensorA:");
  sensorB.readData();
  delay(1500);
  //Serial.println("Reading sensorB:");
  //sensorB.readData();
}

void setupSensors() {
  Wire.begin();
  Wire1.begin();
  Wire2.begin();

  // Assign pins 4 & 3 to SERCOM functionality
  pinPeripheral(W1_SDA, PIO_SERCOM_ALT);
  pinPeripheral(W1_SCL, PIO_SERCOM_ALT);

  // Assign pins 13 & 11 to SERCOM functionality
  pinPeripheral(W2_SDA, PIO_SERCOM);
  pinPeripheral(W2_SCL, PIO_SERCOM);

  delay(2500);

  if (sensorA.checkForSensor()) {
    Serial.println("Found sensor A");
    foundSensor = true;
  } else {
    Serial.println("Could not find sensor A");
  }

  /*
  if (sensorB.checkForSensor()) {
    Serial.println("Found sensor B");
    foundSensor = true;
  } else {
    Serial.println("Could not find sensor B");
  }*/
}
