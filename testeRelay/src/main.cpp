#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(D5, OUTPUT);
}

void loop() {
  Serial.println("fecha");
  digitalWrite(D5, HIGH);
  delay(10000);
  Serial.println("abre");
  digitalWrite(D5, LOW);
  delay(10000);
}