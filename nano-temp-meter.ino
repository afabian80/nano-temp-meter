/*
 * Show the current temperature on an Arduino using a DHT22 sensor
 * and a TM1637 display.
 *
 * Further calibration is possible. Take a measurement with a more
 * accurate device, and set the difference in TEMP_OFFSET.
 *
 * Install TM1637 and DHTNEW libraries in the Arduino library manager.
 *
 */

#include <TM1637Display.h>
#include <dhtnew.h>

// the reference device showed 0.7 C less
#define TEMP_OFFSET -0.7f
#define CLK 4
#define DIO 3
#define DHT_DATA 2
#define FLOAT_TO_INT(x) ((x) >= 0 ? (uint16_t)((x) + 0.5) : (uint16_t)((x)-0.5))

// dht sensor pins: 1:VDD, 2:DATA, 3:GND, 4:GND
DHTNEW mySensor(DHT_DATA);
TM1637Display display(CLK, DIO);
uint16_t temperature;
uint8_t readResult;
uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
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

// showNumber(256) example: show "25_6" on the 4-digit display
void showNumber(uint16_t number) {
  data[0] = display.encodeDigit(number / 100 % 10);
  data[1] = display.encodeDigit(number / 10 % 10);
  data[2] = SEG_D;
  data[3] = display.encodeDigit(number % 10);
  display.setSegments(data);
}

void setup() {
  display.setBrightness(3);
  // immediately show something on power-on for better user experience
  showError();
  // block execution while reading is not ready
  mySensor.setWaitForReading(true);
  // measured difference from better sensor at 27 C
  mySensor.setTempOffset(TEMP_OFFSET);
}

void loop() {
  readResult = mySensor.read();
  // reading is not always successful
  if (readResult == DHTLIB_OK) {
    // drop lower precision digits, multiply by 10 and round to integer
    // e.g. 25.62 -> 256, 25.68 -> 257
    temperature = FLOAT_TO_INT(mySensor.getTemperature() * 10);
    showNumber(temperature);
    errors = 0;
    // refresh roughly every 10s
    delay(10000);
  } else {
    errors = errors + 1;
    // ignore some errors before showing on display
    if (errors > 10) {
      showError();
    }
  }
}
