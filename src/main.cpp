#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

float readCurrent() {
  float current_signal = float(map(analogRead(1), 0, 1023, 0, 509))/100; 
  float current = current_signal / (0.005 * 50);
  return current;
}