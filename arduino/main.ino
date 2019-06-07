#include <Adafruit_NeoPixel.h>
//#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
//#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include <SoftwareSerial.h>
//#include <Wire.h>
#include "DS1307.h"
DS1307 rtc;
#define PIXEL_COUNT 330

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, 7, NEO_GRB + NEO_KHZ800);
//

SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);
Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN, BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);

//
int ALL_HOURS[14][2] = {
  {0, 0},      //
  {138, 150},  //one
  {164, 175},  //two
  {122, 137},  //three
  {176, 189},  //four
  {192, 203},  //five
  {239, 247},  //six
  {221, 238},  //seven
  {206, 220},  //eight
  {248, 258},  //nine
  {262, 271},  //ten
  {272, 288},  //eleven
  {309, 329},  //twelve
  {138, 150}   //one (for past half an hour)
};

int ALL_MINUTES[12][2] = {
  {0, 0},      //
  {80, 90},    //five
  {30, 38},    //ten
  {59, 78},    //quarter
  {41, 57},    //twenty
  {41, 57},    //twenty (five)
  {15, 29},    //half
  {41, 57},    //twenty (five)
  {41, 57},    //twenty
  {59, 78},    //quarter
  {30, 40},    //ten
  {80, 90}     //five
};

int IT[2] = {1, 7};
int IS[2] = {8, 14};
int CURRENT_MINUTE[2] = {0, 0};
int FIVE[2] = {0, 0};
int MINUTES[2] = {0, 0};
int TO_PAST[2] = {0, 0};
int CURRENT_HOUR[2] = {0, 0};
int OCLOCK[2] = {289, 308};
//
int start_red = 255;
int start_green = 0;
int start_blue = 0;
int end_red = 255;
int end_green = 0;
int end_blue = 0;

int h = 5;
int m = 10;
String content;
String cmd;
String inString;
String mode = "solid";
int repeat = 3;
#define ON 1
#define OFF 0

void setup() {
  Serial.begin(115200);
  rtc.begin();
  strip.begin();
  setTime();
  gradient();
  if (!ble.begin(VERBOSE_MODE)) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  ble.echo(false);
  Serial.println("Requesting Bluefruit info:");
  ble.info();
  ble.verbose(false);
  ble.setMode(BLUEFRUIT_MODE_DATA);
  resetNeo();
  Serial.println("Setup complete");
}

void loop() {
  if (ble.available()) {
    int inChar = ble.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    if (inChar == '/') {
      cmd = inString;
      Serial.println(cmd);
      inString = "";
    }
  } else {
    switch (String(cmd.charAt(0)).toInt()) {
      case 1: cmd.remove(0, 1); start_red = cmd.toInt(); gradient(); break;
      case 2: cmd.remove(0, 1); start_green = cmd.toInt(); gradient(); break;
      case 3: cmd.remove(0, 1); start_blue = cmd.toInt(); gradient(); break;
      case 4: cmd.remove(0, 1); end_red = cmd.toInt(); gradient(); break;
      case 5: cmd.remove(0, 1); end_green = cmd.toInt(); gradient(); break;
      case 6: cmd.remove(0, 1); end_blue = cmd.toInt(); gradient(); break;
      case 7: cmd.remove(0, 1); setPattern(cmd.toInt()); break;
      //case 8: cmd.remove(0, 1); setCurrentTime(cmd); break;
      //case 9: cmd.remove(0, 1); repeat = cmd.toInt(); break;
      default: break;
    }
    cmd = "";
  }
}


void setCurrentTime(String data) {
  data.remove(0, 2);
  h = getValue(data, ',', 0).toInt();
  m = getValue(data, ',', 1).toInt();
  Serial.println(h);
  Serial.println(m);
  rtc.fillByHMS(h, m, 0);
  rtc.setTime();
  setTime();
}

void setTime() {
  m /= 5;

  if (m == 0 || m == 3 || m == 6 || m == 9) {                     //If minute is 0, 15, 30, or 45           //
    SET_MINUTES(OFF);                                             //Turn off word "MINUTES"                 //
  } else {                                                        //If minute equals 1, 2, 4, 5, 7, or 8    //
    SET_MINUTES(ON);                                              //Turn on word "MINUTES"                  //
  }

  if (m == 0) {                                                   //If minute is zero                       //
    SET_TO(OFF);                                                  //Turn off word "TO"                      //
  } else if (m <= 6) {                                            //If minute is less than or equal to 30   //
    SET_PAST(ON);                                                 //Turn on word "PAST"                     //
    SET_HOUR(h);                                                  //Set the current hour                    //
  } else if (m > 6) {                                             //If minute is more than 30               //
    SET_TO(ON);                                                   //Turn on word "TO"                       //
    SET_HOUR(h + 1);                                              //Set one hour ahead for "TO" logic       //
  }                                                                                                         //

  if (m == 5 || m == 7) {                                         //If minute is 25 or 35, turn on "FIVE"   //
    SET_MINUTE(m);                                                //Set current minute                      //
    SET_FIVE(ON);                                                 //Turn on word "FIVE"                     //
  } else {                                                        //If minute doesn't equal 25 or 35, set   //
    SET_MINUTE(m);                                                //Set current minute                      //
    SET_FIVE(OFF);                                                //Turn off word "FIVE"                    //
  }                                                                                                         //
}

void show(int led, int red, int green, int blue) {
  if (led >= IT[0] && led <= IT[1] ||                          //IT
      led >= IS[0] && led <= IS[1] ||                          //IS
      led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
      led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
      led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
      led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
      led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
      led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
    strip.setPixelColor(led, red, green, blue);
  } else {
    strip.setPixelColor(led, 0, 0, 0);
  }
}

void SET_MINUTES(int s) {
  if (s) {
    MINUTES[0] = 92;
    MINUTES[1] = 113;
  } else {
    MINUTES[0] = 0;
    MINUTES[1] = 0;
  }
}

void SET_TO(int s) {
  if (s) {
    TO_PAST[0] = 114;
    TO_PAST[1] = 121;
  } else {
    TO_PAST[0] = 0;
    TO_PAST[1] = 0;
  }
}

void SET_PAST(int s) {
  if (s) {
    TO_PAST[0] = 151;
    TO_PAST[1] = 162;
  } else {
    TO_PAST[0] = 0;
    TO_PAST[1] = 0;
  }
}

void SET_FIVE(int s) {
  if (s) {
    FIVE[0] = 80;
    FIVE[1] = 90;
  } else {
    FIVE[0] = 0;
    FIVE[1] = 0;
  }
}

void SET_MINUTE(int m) {
  CURRENT_MINUTE[0] = ALL_MINUTES[m][0];
  CURRENT_MINUTE[1] = ALL_MINUTES[m][1];
}

void SET_HOUR(int h) {
  CURRENT_HOUR[0] = ALL_HOURS[h][0];
  CURRENT_HOUR[1] = ALL_HOURS[h][1];
}

void setPattern(int command) {
  Serial.println("setPattern();");
  if (command == (0)) {
    rainbow();
  }
  if (command == (1)) {
    r2g();
  }
  if (command == (2)) {
    r2g();
  }
  if (command == (3)) {
    r2b();
  }
  if (command == (4)) {
    g2r();
  }
  if (command == (5)) {
    g2b();
  }
  if (command == (6)) {
    b2r();
  }
  if (command == (7)) {
    b2g();
  }
}

void gradient() {
  resetNeo();
  Serial.println("Running gradient();");
  for (int led = 0; led < PIXEL_COUNT; led++) {
    int newRed;
    int newGreen;
    int newBlue;

    if (start_red != end_red) {
      if (start_red < end_red) {
        newRed = round(start_red + (led * 0.77));
      } else {
        newRed = round(start_red - (led * 0.77));
      }
    } else {
      newRed = start_red;
    }
    if (start_green != end_green) {
      if (start_green < end_green) {
        newGreen = round(start_green + (led * 0.77));
      } else {
        newGreen = round(start_green - (led * 0.77));
      }
    } else {
      newGreen = start_green;
    }
    if (start_blue != end_blue) {
      if (start_blue < end_blue) {
        newBlue = round(start_blue + (led * 0.77));
      } else {
        newBlue = round(start_blue - (led * 0.77));
      }
    } else {
      newBlue = start_blue;
    }
    show(led, newRed, newGreen, newBlue);
  }
  strip.show();
}

void r2g() {
  for (int r = 0; r < repeat; r++) {
    int newRed = 255;
    int newGreen = 0;
    for (int i = 0; i < 255; i++) {
      newRed--;
      newGreen++;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, newGreen, 0);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newRed++;
      newGreen--;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, newGreen, 0);
      }
      strip.show();
    }
  }
  gradient();
}

void r2b() {
  for (int r = 0; r < repeat; r++) {
    int newRed = 255;
    int newBlue = 0;
    for (int i = 0; i < 255; i++) {
      newRed--;
      newBlue++;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, 0, newBlue);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newRed++;
      newBlue--;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, 0, newBlue);
      }
      strip.show();
    }
  }
  gradient();
}

void g2r() {
  for (int r = 0; r < repeat; r++) {
    int newGreen = 255;
    int newRed = 0;
    for (int i = 0; i < 255; i++) {
      newGreen--;
      newRed++;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, newGreen, 0);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newGreen++;
      newRed--;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, newGreen, 0);
      }
      strip.show();
    }
  }
  gradient();
}

void g2b() {
  for (int r = 0; r < repeat; r++) {
    int newGreen = 255;
    int newBlue = 0;
    for (int i = 0; i < 255; i++) {
      newGreen--;
      newBlue++;
      for (int led = 0; led < 330; led++) {
        show(led, 0, newGreen, newBlue);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newGreen++;
      newBlue--;
      for (int led = 0; led < 330; led++) {
        show(led, 0, newGreen, newBlue);
      }
      strip.show();
    }
  }
  gradient();
}

void b2g() {
  for (int r = 0; r < repeat; r++) {
    int newBlue = 255;
    int newGreen = 0;
    for (int i = 0; i < 255; i++) {
      newGreen--;
      newBlue++;
      for (int led = 0; led < 330; led++) {
        show(led, 0, newGreen, newBlue);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newBlue++;
      newGreen--;
      for (int led = 0; led < 330; led++) {
        show(led, 0, newGreen, newBlue);
      }
      strip.show();
    }
  }
  gradient();
}

void b2r() {
  for (int r = 0; r < repeat; r++) {
    int newBlue = 255;
    int newRed = 0;
    for (int i = 0; i < 255; i++) {
      newBlue--;
      newRed++;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, 0, newBlue);
      }
      strip.show();
    }
    for (int i = 0; i < 255; i++) {
      newBlue++;
      newRed--;
      for (int led = 0; led < 330; led++) {
        show(led, newRed, 0, newBlue);
      }
      strip.show();
    }
  }
  gradient();
}

void all() {
  for (int led = 0; led < PIXEL_COUNT; led++) {
    strip.setPixelColor(led, 255, 255, 255);
  }
  strip.show();
}

void rainbow() {
  resetNeo();
  Serial.println("Running rainbow();");
  uint16_t led;
  for (int j = 0; j < 256 * repeat; j++) {
    for (led = 0; led < strip.numPixels(); led++) {
      if (led >= IT[0] && led <= IT[1] ||                          //IT
          led >= IS[0] && led <= IS[1] ||                          //IS
          led >= CURRENT_MINUTE[0] && led <= CURRENT_MINUTE[1] ||  //FIVE/TEN/QUARTER/TWENTY/HALF
          led >= FIVE[0] && led <= FIVE[1] ||                      //FIVE
          led >= MINUTES[0] && led <= MINUTES[1] ||                //MINUTES
          led >= TO_PAST[0] && led <= TO_PAST[1] ||                //TO/PAST
          led >= CURRENT_HOUR[0] && led <= CURRENT_HOUR[1] ||      //ONE/TWO/THREE/FOUR/FIVE/SIX/SEVEN/EIGHT/NINE/TEN/ELEVEN/TWELVE
          led >= OCLOCK[0] && led <= OCLOCK[1]) {                  //OCLOCK
        strip.setPixelColor(led, Wheel(((led * 256 / PIXEL_COUNT) + j) & 255));
      } else {
        strip.setPixelColor(led, 0, 0, 0);
      }
    }
    strip.show();
  }
}

void rainbowAll() {
  uint16_t led, j;
  for (j = 0; j < 256 * 10; j++) {
    for (led = 0; led < strip.numPixels(); led++) {
      strip.setPixelColor(led, Wheel(((led * 256 / PIXEL_COUNT) + j) & 255));
    }
    strip.show();
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void fadeInOut() {
  float red, green, blue;
  for (int i = 0; i < repeat; i++) {
    for (int k = 0; k < 256; k = k + 1) {
      red = (k / 256.0) * 255;
      green = (k / 256.0) * 0;
      blue = (k / 256.0) * 0;
      for (int led = 0; led < 330; led++) {
        show(led, red, green, blue);
      }
      strip.show();
    }
    for (int k = 255; k >= 0; k = k - 2) {
      red = (k / 256.0) * 255;
      green = (k / 256.0) * 255;
      blue = (k / 256.0) * 255;
      for (int led = 0; led < 330; led++) {
        show(led, red, green, blue);
      }
      strip.show();
    }
  }
}

void resetNeo() {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

/* OTHER */
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void error(const __FlashStringHelper * err) {
  Serial.println(err);
  while (1);
}
