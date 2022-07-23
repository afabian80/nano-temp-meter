#include <TM1637Display.h>  // TM1637 lib
#include <dhtnew.h>  // DHTNEW lib

#define FLOAT_TO_INT(x) ((x)>=0?(uint16_t)((x)+0.5):(uint16_t)((x)-0.5))

#define CLK 4
#define DIO 3
#define DHT_DATA 2

/*
dht sensor pins: 1:VDD, 2:DATA, 3:GND, 4:GND
*/

DHTNEW mySensor(DHT_DATA);
TM1637Display display(CLK,DIO);
uint16_t temperature;
uint8_t readResult;
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t errors = 0;

// showError() shows "----" on the 4-digit display
void showError() {
  data[0] = SEG_G;
  data[1] = SEG_G;
  data[2] = SEG_G;
  data[3] = SEG_G;
  display.setSegments(data);
  delay(500);
}

// showNumber(...) example: show 256 as "25_6" on the 4-digit display
void showNumber(uint16_t number) {
  data[0] = display.encodeDigit(number / 100 % 10);
  data[1] = display.encodeDigit(number / 10 % 10);
  data[2] = SEG_D;
  data[3] = display.encodeDigit(number % 10);
  display.setSegments(data);
}

void setup() {
  display.setBrightness(3);
  showError();  // immediately show something on power-on, better user experience
  mySensor.setWaitForReading(true);
  mySensor.setTempOffset(0);  // diff from better sensor at 22 C, TODO: measure it
}

void loop() {
  readResult = mySensor.read();
  if(readResult == DHTLIB_OK) {  // not always successful
    temperature = FLOAT_TO_INT(mySensor.getTemperature() * 10);  // drop lower precision, 25.62 -> 256, 25.68 -> 257
    showNumber(temperature);
    errors = 0;
    delay(10000);
  } else {
    errors = errors + 1;
    if(errors > 10) {  // ignore some errors before showing on display
      showError();
    }
  }
}
