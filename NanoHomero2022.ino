#include <TM1637Display.h>  // TM1637 lib
#include <dhtnew.h>  // DHTNEW lib

#define CLK 4
#define DIO 3
#define DHT_DATA 2

/*
dht sensor pins: 1:VDD, 2:DATA, 3:GND, 4:GND
*/

DHTNEW mySensor(DHT_DATA);
TM1637Display display(CLK,DIO);
uint16_t temp;
uint8_t readResult;
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t errors = 0;

void setup() {
  mySensor.setWaitForReading(true);
  display.setBrightness(3);
}

void loop() {
  readResult = mySensor.read();
  if(readResult == DHTLIB_OK) {
    temp = mySensor.getTemperature() * 10 - 9;  // diff from better sensor at 22 C
    data[0] = display.encodeDigit(temp / 100 % 10);
    data[1] = display.encodeDigit(temp / 10 % 10);
    data[2] = SEG_D;
    data[3] = display.encodeDigit(temp % 10);
    errors = 0;
    delay(10000);
  } else {
    errors = errors + 1;
    if(errors > 10) {
      data[0] = SEG_G;
      data[1] = SEG_G;
      data[2] = SEG_G;
      data[3] = SEG_G;
    }
  }
  display.setSegments(data);
}
