#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <HotBox.h>
// put function declarations here:

HotBox box = HotBox(1);
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  can1.begin();
  can1.setBaudRate(250000);
  box.start(&can1);
  
}

void loop() {
  box.readValues();
}
