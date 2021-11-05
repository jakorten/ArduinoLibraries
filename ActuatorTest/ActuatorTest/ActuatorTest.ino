/*
 * Actuator Tester sketch by Johan Korten
 * November 5, 2021
 */

#define pump   6
#define valveA 5
#define valveB 7
#define valveC 9

void setup() {
  // put your setup code here, to run once:
  pinMode(valveA, OUTPUT);
  pinMode(valveB, OUTPUT);
  pinMode(valveC, OUTPUT);
  pinMode(pump, OUTPUT);    
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(valveA, HIGH);
  digitalWrite(valveB, LOW);
  digitalWrite(valveC, LOW);
  digitalWrite(pump, LOW);
  delay(1500);

  digitalWrite(valveA, LOW);
  digitalWrite(valveB, HIGH);
  digitalWrite(valveC, LOW);
  digitalWrite(pump, LOW);
  delay(1500);

  digitalWrite(valveA, LOW);
  digitalWrite(valveB, LOW);
  digitalWrite(valveC, HIGH);
  digitalWrite(pump, LOW);
  delay(1500);

  digitalWrite(valveA, LOW);
  digitalWrite(valveB, LOW);
  digitalWrite(valveC, LOW);
  digitalWrite(pump, HIGH);
  delay(1500);
}
