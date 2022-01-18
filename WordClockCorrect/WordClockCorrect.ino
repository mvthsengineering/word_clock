//Instructions:
//General Bluetooth:
//1) Download the Bluefruit Connect app
//2) Connect to the "Adafruit Bluefruit LE" in the Bluefruit Connect app
//Setting the Time:
//1) Open the "UART" section in the Bluefruit Connect app
//2) To set the hour, type "h#", where "#" is the number of the hour you want to set the clock to, and hit send
//3) To set the minute, type "m#", where "#" is the number of the minute you want to set the clock to, and hit send
//4) To set the second, type "s#", where "#" is the number of the second you want to set the clock to, and hit send
//5) To push that data to the RTC type "t" and hit send
//NOTE: Please DO NOT set a time that would not appear on an analog clock.
//Setting the Color:
//1) Open the "UART" section in the Bluefruit Connect app
//2) To set the red value, type "r#", where "#" is the number you want to set the red value to, and hit send
//3) To set the green value, type "g#", where "#" is the number you want to set the green value to, and hit send
//4) To set the blue value, type "b#", where "#" is the number you want to set the blue value to, and hit send
//5) To set a random color value, type "q" and hit send
//6) To push that data to the LEDs, type "c" and hit send
//NOTE: You may need to imput commands several times before they register. The command has only registered once a value is returned.

//Defines, includes, and global variables
//RTC section
#include "RTClib.h"
RTC_PCF8523 rtc;
int current_hr;
int current_min;
int current_sec;
//Neopixel section
#include <Adafruit_NeoPixel.h>
#define PIN        2
#define NUMPIXELS 132
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//Bluefruit section
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"
#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
extern uint8_t packetbuffer[];
uint8_t red = 100;
uint8_t green = 100;
uint8_t blue = 100;

//What the bluetooth does when there is an error
void error(const __FlashStringHelper*err) {     //Defines the function and adds a value to be passed into the function
  Serial.println(err);                          //Prints the value that is passed into the function
  while (1);                                    //Does nothing until the loop is broken
}

//Contains the LED values in each row of the clock
//Be aware that the LEDs on the board are numbered in a snake pattern
int leds[11][12] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},                           // ----->
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},                 // <-----
  {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},                 // ----->
  {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},                 // <-----
  {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59},                 // ----->
  {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71},                 // <-----
  {72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},                 // ----->
  {84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},                 // <-----
  {96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107},         // ----->
  {108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119},     // <-----
  {120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131}      // ----->
};

//Contains the LED values for the minutes
int mins[12][2] = {
  { -1, -1},         //Empty value for when it's just an hour
  {30, 33},          //Five
  {38, 40},          //Ten
  {15, 21},          //Fifteen
  {24, 29},          //Twenty
  {24, 33},          //Twentyfive
  {42, 47},          //Thirty
  {24, 33},          //Twentyfive
  {24, 29},          //Twenty
  {15, 21},          //Fifteen
  {38, 40},          //Ten
  {30, 33}           //Five
};

//Contains the LED values for the hours
int hrs[24][2] = {
  {78, 83},          //Twelve
  {109, 111},        //One
  {84, 86},          //Two
  {112, 116},        //Three
  {61, 64},          //Four
  {88, 91},          //Five
  {117, 119},        //Six
  {72, 76},          //Seven
  {96, 100},         //Eight
  {92, 95},          //Nine
  {120, 122},        //Ten
  {102, 107},        //Eleven
  {78, 83},          //Twelve
  {109, 111},        //One
  {84, 86},          //Two
  {112, 116},        //Three
  {61, 64},          //Four
  {88, 91},          //Five
  {117, 119},        //Six
  {72, 76},          //Seven
  {96, 100},         //Eight
  {92, 95},          //Nine
  {120, 122},        //Ten
  {102, 107}         //Eleven
};

//Contains the LED values for the extra words
int extras[6][2] = {
  {0, 1},            //It
  {3, 4},            //Is
  {126, 131},        //O'clock
  {48, 54},          //Minutes
  {68, 71},          //Past
  {67, 68}           //To
};

//Contains a set of values for every five minutes
int intv[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};

void setup() {                                                                               //Runs once when the code is initiated
  Serial.begin(115200);                                                                       //Starts the serial monitor
  if ( !ble.begin(VERBOSE_MODE) ) {                                                           //For when the bluetooth reciever fails to start
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));       //Runs the "error" function (found below the defines, includes, and global variables section)
  }
  Serial.println( F("OK!") );                                                                 //Prints "OK!" when the bluetooth reciever has successfully started
  ble.echo(false);                                                                            //Disables echo output and prints its state
  ble.verbose(false);                                                                         //Disables debug output
  ble.setMode(BLUEFRUIT_MODE_DATA);                                                           //Sets the bluetooth reciever to data recieving mode
  Serial.println(":)");                                                                       //Prints ":)" when the bluetooth reciever is finished starting
  if (! rtc.begin()) {                                                                        //For when the clock fails to start
    Serial.flush();                                                                            //Sends all of the backed up data
    abort();                                                                                   //Stops the program
  }
  if (! rtc.initialized() || rtc.lostPower()) {                                               //For when the clock is loaded or loses and regains power
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));                                            //Syncs the clock's time with computer's time
  }
  rtc.start();                                                                                //Starts the clock
  pixels.begin();                                                                             //Sets up the LEDs
  pixels.clear();                                                                             //Turns off all of the LEDs
  pixels.setPixelColor(20, pixels.Color(200, 200, 255));                                      //Sets the 20th pixel as a test to see if the Neopixel is recieving data
  pixels.show();                                                                              //Turns on the 20th Neopixel
}

//Changes the minute
void change_minute(int this_minute, int this_second) {                     //Defines the function and adds values that can be passed into the function
  for (int i = mins[this_minute][0]; i <= mins[this_minute][1]; i++) {      //Sets up LEDs for the corresponding five minute interval
    pixels.setPixelColor(i, pixels.Color(red, green, blue));                 //Sets the color of the LEDs
  }
}

//Changes the hour
void change_hour(int this_hour, int this_minute, int this_second) {             //Defines the function and adds values that can be passed into the function
  if (this_minute < intv[7]) {                                                   //For when it's less than 35 minutes into the hour
    for (int i = hrs[this_hour][0]; i <= hrs[this_hour][1]; i++) {                //Sets up the LEDs for the corresponding hour
      pixels.setPixelColor(i, pixels.Color(red, green, blue));                     //Sets the color of the LEDs
    }
  } else {                                                                       //For when it's greater than 35 minutes into the hour
    for (int i = hrs[this_hour + 1][0]; i <= hrs[this_hour + 1][1]; i++) {        //Sets up the LEDs for the corresponding hour
      pixels.setPixelColor(i, pixels.Color(red, green, blue));                     //Sets the color of the LEDs
    }
  }
}

//Changes the extra words
void change_extras(int this_minute) {                           //Defines the function and adds values that can be passed into the function
  int extra_words;                                               //Sets a variable for "extra_words"
  if (this_minute >= 5) {                                        //For when the time is not on an hour (greater than or equal to 5 minutes into the hour)
    if (this_minute < intv[7]) {                                  //For when it's less than 35 minutes into the hour
      extra_words = 4;                                             //Sets up the LEDs for when the words "It", "Is", "Minutes", "Past", and "O'clock" should show
    } else {                                                      //For when it's not less than 35 minutes into the hour
      extra_words = 3;                                             //Sets up the LEDs for when the words "It", "Is", "Minutes", and "O'clock" should show
    }
  } else {                                                       //For when the time is on an hour (less than 5 minutes into the hour)
    extra_words = 2;                                              //Sets up for the LEDs for when the words "It", "Is", and "O'clock" should show
  }
  for (int i = 0; i <= extra_words; i++) {                       //Sets up the LEDs for the corresponding number of extra words that need to appear
    for (int j = extras[i][0]; j <= extras[i][1]; j++) {          //Sets up LEDs for the corresponding extra words
      pixels.setPixelColor(j, pixels.Color(red, green, blue));     //Sets the color of the LEDs
    }
  }
  if (this_minute >= intv[7]) {                                  //For when it's less than 35 minutes into the hour
    for (int i = extras[5][0]; i <= extras[5][1]; i++) {          //Sets up the LEDs for when the word "To" should show
      pixels.setPixelColor(i, pixels.Color(red, green, blue));     //Sets the color of the LEDs
    }
  }
}

//Lights up the LEDs
void show_pixels() {                              //Defines the function
  DateTime now = rtc.now();                        //Allows for "now." functions to be called
  int time_sec = now.second();                     //Sets a variable to the current second
  int time_min = now.minute();                     //Sets a variable to the current minute
  int time_hr = now.hour();                        //Sets a variable to the current hour
  if ((time_min % 5) == 0) {                       //For when the time is on a five minute interval
    pixels.clear();                                 //Turns off all of the LEDs
    change_minute((time_min / 5), time_sec);        //Calls the minute changinng function and passes the current time into it
    change_hour(time_hr, time_min, time_sec);       //Calls the hour changing function and passes the current time into it
    change_extras(time_min);                        //Calls the extra word changing function and passes the current time into it
    pixels.show();                                  //Turns on the LEDs that were called for in the previous three functions
  }
}

//Sets the time
void set_time() {
  DateTime now = rtc.now();                                                   //Allows for "now." functions to be called
  ble.readline();                                                             //Reads the data sent from the phone
  if (ble.buffer[0] == 'h') {                                                 //For when the first character in the message is "h"
    if (ble.buffer[2] != 0) {                                                  //For when there is a third character in the message
      current_hr = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);          //Sets the current hour to the value of the second character minus forty-eight multiplied by ten plus the value of the third character minus forty-eight
    } else {                                                                   //For when there is not a third character in the message
      current_hr = ble.buffer[1] - 48;                                          //Sets the current hour to the value of the second character minus forty-eight
    }
    ble.println(current_hr);                                                     //Prints the hour that is being set on the phone
  } else if (ble.buffer[0] == 'm') {                                          //For when the fist character in the message is "m"
    if (ble.buffer[2] != 0) {                                                  //For when there is a third character in the message
      current_min = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);         //Sets the current minute to the value of the second character minus forty-eight multiplied by ten plus the value of the third character minus forty-eight
    } else {                                                                   //For when there is not a third character
      current_min = ble.buffer[1] - 48;                                         //Sets the current minute to the value of the second character minus forty-eight
    }
    ble.println(current_min);                                                    //Prints the minute that is being set on the phone
  } else if (ble.buffer[0] == 's') {                                          //For when the fist character in the message is "s"
    if (ble.buffer[2] != 0) {                                                  //For when there is a third character in the message
      current_sec = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);         //Sets the current second to the value of the second character minus forty-eight multiplied by ten plus the value of the third character minus forty-eight
    } else {                                                                   //For when there is not a third character in the mmessage
      current_sec = ble.buffer[1] - 48;                                         //Sets the current second to the value of the second character minus forty-eight
    }
    ble.println(current_sec);                                                    //Prints the second that is being set on the phone
  } else if (ble.buffer[0] == 't') {                                          //For when the first character in the message is "t"
    ble.print(current_hr);                                                     //Prints the hour that is being set on the phone
    ble.print(": ");                                                           //Prints a colon and a space on the phone
    ble.print(current_min);                                                    //Prints the minute that is being set on the phone
    ble.print(": ");                                                           //Prints a colon and a space on the phone
    ble.println(current_sec);                                                  //Prints the minute that is being set on the phone
    rtc.adjust(DateTime(2021, 1, 1, current_hr, current_min, current_sec));    //Sets the time to the hour, minute, and second that were previously set through the phone
    rtc.start();                                                               //Starts the clock
    pixels.clear();                                                            //Turns off all of the LEDs
    pixels.show();                                                             //Turns on the LEDs called for in the previous functions
  }
}

//Sets the color
void set_color() {                                                                                  //Defines the function
  DateTime now = rtc.now();                                                                          //Allows for "now." functions to be called
  ble.readline();                                                                                    //Reads the data sent from the phone
  if (ble.buffer[0] == 'r') {                                                                        //For when the first character in the message is "r"
    if (ble.buffer[2] != 0) {                                                                         //For when there is a third character in the message
      if (ble.buffer[3] != 0) {                                                                        //For when there is a fourth character in the message
        red = ((ble.buffer[1] - 48) * 100) + ((ble.buffer[2] - 48) * 10) + (ble.buffer[3] - 48);        //Sets the red variable to the value of the second character minus forty-eight multiplied by one-hundred plus the value of the third character minus forty-eight multiplied by ten plus the value of the fourth character minus forty-eight
      } else {                                                                                         //For when there is not a fourth character in the message
        red = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);                                       //Sets the red variable to the value of the second character minus forty-eight multiplied by ten plus value of the third character minus forty-eight
      }
    } else {                                                                                          //For when there is not a thrid character in the message
      red = ble.buffer[1] - 48;                                                                        //Sets the red variable to the value of the second character minus forty-eight
    }
    ble.println(red);                                                                                 //Prints the red value on the phone
  } else if (ble.buffer[0] == 'g') {                                                                 //For when the first character of the message is "g"
    if (ble.buffer[2] != 0) {                                                                         //For when there is a third character in the message
      if (ble.buffer[3] != 0) {                                                                        //For when there is a fourth character in the message
        green = ((ble.buffer[1] - 48) * 100) + ((ble.buffer[2] - 48) * 10) + (ble.buffer[3] - 48);      //Sets the green variable to the value of the second character minus forty-eight multiplied by one-hundred plus the value of the third character minus forty-eight multiplied by ten plus the value of the fourth character minus forty-eight
      } else {                                                                                         //For when there is not a fourth character in the message
        green = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);                                     //Sets the green variable to the value of the second character minus forty-eight minus forty-eight multiplied by ten plus the value of the third character minue forty-eight
      }
    } else {                                                                                          //For when there is not a third character in the message
      green = ble.buffer[1] - 48;                                                                      //Sets the green variable to the value of the second character minus forty-eight
    }
    ble.println(green);                                                                               //Prints the green value on the phone
  } else if (ble.buffer[0] == 'b') {                                                                 //For when the first character of the message is "b"
    if (ble.buffer[2] != 0) {                                                                         //For when there is a third character in the message
      if (ble.buffer[3] != 0) {                                                                        //For when there is a fourth character in the messsage
        blue = ((ble.buffer[1] - 48) * 100) + ((ble.buffer[2] - 48) * 10) + (ble.buffer[3] - 48);       //Sets the blue variable to the value of the second character minus forty-eight multiplied by one-hundred plus the value of the third character minus forty-eight multiplied by ten plus the value of the fourth character minus forty-eight
      } else {                                                                                         //For when there is not a fourth character in the message
        blue = ((ble.buffer[1] - 48) * 10) + (ble.buffer[2] - 48);                                      //Sets the blue variable to the value of the second character minus forty-eight multiplied by ten plus the value of the third character minus forty-eight
      }
    } else {                                                                                          //For when there is not a third character in the message
      blue = ble.buffer[1] - 48;                                                                       //Sets the blue variable to the value of the second character minus forty-eight
    }
    ble.println(blue);                                                                                //Prints the blue value on the phone
  } else if (ble.buffer[0] == 'q') {                                                                 //For when the first character of the message is "q"
    red = random(255);                                                                                //Sets the red variable to a random value from zero to two-hundred-fifty-five
    green = random(255);                                                                              //Sets the green variable to a random value from zero to two-hundred-fifty-five
    blue = random(255);                                                                               //Sets the blue variable to a random value from zero to two-hundred-fifty-five
    ble.print(red);                                                                                   //Prints the red value on the phone
    ble.print(": ");                                                                                  //Prints a colon and a space on the phone
    ble.print(green);                                                                                 //Prints the green value on the phone
    ble.print(": ");                                                                                  //Prints a colon and a space on the phone
    ble.println(blue);                                                                                //prints the blue value on the phone
  } else if (ble.buffer[0] == 'c') {                                                                 //For when the first character of the message is "c"
    int time_hr = now.hour();                                                                         //Sets a variable to the current hour
    int time_min = now.minute();                                                                      //Sets a variable to the current minute
    int time_sec = now.second();                                                                      //Sets a variable to the current second
    ble.print(red);                                                                                   //Prints the red value on the phone
    ble.print(": ");                                                                                  //Prints a colon and a space on the phone
    ble.print(green);                                                                                 //Prints the green value on the phone
    ble.print(": ");                                                                                  //prints a colon annd a space on the phone
    ble.println(blue);                                                                                //Prints the blue value on the phone
    rtc.start();                                                                                      //Starts the clock
    pixels.clear();                                                                                   //Turns off all of the LEDs
    change_minute((time_min / 5), time_sec);                                                          //Calls the minute changinng function and passes the current time into it
    change_hour(time_hr, time_min, time_sec);                                                         //Calls the hour changing function and passes the current time into it
    change_extras(time_min);                                                                          //Calls the extra word changing function and passes the current time into it
    pixels.show();                                                                                    //Turns on the LEDs that were called for in the previous three functions
  }
}

void loop() {        //Executes repeatedly
  set_time();         //Sets the time of the RTC
  set_color();        //Sets the color of the LEDs
  show_pixels();      //Lights up the LEDs
}
