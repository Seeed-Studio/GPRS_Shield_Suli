/*
GPRS Call Up 

This sketch is used to test seeeduino GPRS_Shield's callUp function.
to make it work, you should insert SIM card to Seeeduino GPRS 
and replace the phoneNumber,enjoy it!

create on 2013/12/5, version: 0.1
by lawliet.zou(lawliet.zou@gmail.com)
*/
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Suli.h>

#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  9600
#define PHONE_NUMBER  "186****2964"

GPRS gprsTest(PIN_TX,PIN_RX,BAUDRATE);//RX,TX,PWR,BaudRate

void setup() {
  Serial.begin(9600);
  while(0 != gprsTest.init()) { //gprs init
      delay(1000);
      Serial.print("init error\r\n");
  }
  Serial.println("gprs init success");
  Serial.println("start to call ...");
  gprsTest.callUp(PHONE_NUMBER);
}

void loop() {
  //nothing to do
}
