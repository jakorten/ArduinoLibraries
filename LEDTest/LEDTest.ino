/*
  Blink without Delay

  Turns on and off a light emitting diode (LED) connected to a digital pin,
  without using the delay() function. This means that other code can run at the
  same time without being interrupted by the LED code.

  The circuit:
  - Use the onboard LED.
  - Note: Most Arduinos have an on-board LED you can control. On the UNO, MEGA
    and ZERO it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN
    is set to the correct LED pin independent of which board is used.
    If you want to know what pin the on-board LED is connected to on your
    Arduino model, check the Technical Specs of your board at:
    https://www.arduino.cc/en/Main/Products

  created 2005
  by David A. Mellis
  modified 8 Feb 2010
  by Paul Stoffregen
  modified 11 Nov 2013
  by Scott Fitzgerald
  modified 9 Jan 2017
  by Arturo Guadalupi
  modified 5 Nov 2021
  by Johan Korten

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
*/

// constants won't change. Used here to set a pin number:
#define healthLED 12
#define heartBeatLED 14
#define progLED 2

// Variables will change:
int ledStateHB = LOW;             // ledState used to set the LED
int ledStateHealth = LOW;
int ledStateProg = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillisHB = 0;        // will store last time LED was updated
unsigned long previousMillisHealth = 0;
unsigned long previousMillisProg = 0;

// constants won't change:
const long intervalHB = 100;           // interval at which to blink (milliseconds)
const long intervalHealth = 500;           // interval at which to blink (milliseconds)
const long intervalProg = 100;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(healthLED, OUTPUT);
  pinMode(heartBeatLED, OUTPUT);
  pinMode(progLED, OUTPUT);
}

void loop() {
  updateHeartBeat();
  updateHealthLed();
  updateProgLed();
}

void updateProgLed() {
  updateStateAsynch(previousMillisProg, intervalProg, ledStateProg, progLED);
}

void updateHeartBeat() {
  updateStateAsynch(previousMillisHB, intervalHB, ledStateHB, heartBeatLED);
}

void updateHealthLed() {
  updateStateAsynch(previousMillisHealth, intervalHealth, ledStateHealth, healthLED);
}

void updateStateAsynch(unsigned long &prevMillis, const int interval, int &ledState, const int ledPin) {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis >= interval) {
    // save the last time you blinked the LED
    prevMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}
