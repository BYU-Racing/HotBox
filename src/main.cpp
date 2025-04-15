#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <HotBox.h>
// put function declarations here:

HotBox box = HotBox(1);
HotBox box2 = HotBox(2);

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  can1.begin();
  can1.setBaudRate(250000);
  delay(1000);
  Serial.println("START");
  box.start(&can1, 2);
  box2.start(&can1, 1);
  Serial.println("Both Camera INIT Successful");
}

void loop() {
  box.readValues();
  box2.readValues();
}
