#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <HotBox.h>
// put function declarations here:

HotBox box = HotBox(1);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  box.start();
  
}

void loop() {
  box.readValues();
}
