#include <Arduino.h>
// Note: installed these libraries with platformio library installer
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* Software SPI (slower updates, more flexible pin options):
 * Arduino pin 13 - Serial clock out (SCLK)
 * Arduino pin 11 - Serial data out (DN)
 * Arduino pin 5 - Data/Command select (D/C)
 * Arduino pin 6 - LCD reset (RST)
 * Arduino pin 7 - LCD chip enable (SCE)
 * Harware SPI not working => Adafruit_PCD8544 display = Adafruit_PCD8544(5, 7 ,6);
 */
Adafruit_PCD8544 display = Adafruit_PCD8544 (13, 11, 5, 7, 6);

/* define hardware pins used
 *
 * Arduino pin  8  -> LH switch (up)
 * Arduino pin  9  -> RH switch (down)
 * Arduino pin  10 -> pwm pin for lcd brightness
 * Arduino pin  A0 -> solar panel
 * Arduino pin  A1 -> pot 
 */
#define UP_SWITCH 8
#define DOWN_SWITCH 9
#define LCD_BACKLIGHT 10 
#define SOLAR_CELL_INPUT A0
#define VARIABLES_POT A1

// define some useful constants
#define DELAY_TIME 100
#define LCD_CONTRAST 50
#define LCD_DEFAULT_BRIGHTNESS 127
#define LCD_TEXT_SIZE 1
#define LCD_ROTATION 2

#define numberOfColumns 8;

struct position {
  int x;
  int y;
};

// function prototypes
int getLightReading (float, float, float, float);
void lcdprint (position, String);
float evCalibrated (float);

void setup() {
//  SPI.setClockDivider(SPI_CLOCK_DIV16); // doesn't work with this library
  pinMode (UP_SWITCH, INPUT);
  pinMode (DOWN_SWITCH, INPUT);
  pinMode (LCD_BACKLIGHT, OUTPUT);
  Serial.begin(9600);

  display.begin(); // initialize display
  analogWrite (LCD_BACKLIGHT, LCD_DEFAULT_BRIGHTNESS);
  display.setContrast (LCD_CONTRAST);
  display.setTextSize (LCD_TEXT_SIZE);
  display.setRotation (LCD_ROTATION);
  display.clearDisplay ();
}

void loop() {
  float iso = 100;
  float fstop = 16;
  float shutter = 1/100;
  float evAdjust = 0.0;
  int lightReading;
  float ev;
  position lineOnePosition = { 0, 0 };
  position lineTwoPosition = { 0, 10 };
  position lineOneEraseFromPosition = { 18, 0 };

  display.clearDisplay (); // how can this be avoided?
  lightReading = getLightReading (iso, fstop, shutter, evAdjust);
  lcdprint (lineOnePosition, "LR "+String (lightReading) );
  ev = evCalibrated ( float (lightReading) );
  lcdprint (lineTwoPosition, "EV "+String(ev) );
  delay (DELAY_TIME);

}

/*********************************************
 * get the light reading from the solar panel 
 *********************************************/
int getLightReading (float filmSpeed, float aperture, float shutterSpeed, float evCompensation){

  #define MIN_LIGHT 0
  #define MAX_LIGHT 360
  #define MIN_EV 0
  #define MAX_EV 100
  int light = analogRead (SOLAR_CELL_INPUT);
  return light;
  // later use this: return evCalibrated ( light ); 
}

/*********************************************
 * print message to lcd panel at coordinate 
 ********************************************clearFrom.x, clearFrom.y+*/
void lcdprint (position coordinate, String message) {
  display.setCursor (coordinate.x, coordinate.y);
  display.println (message);
  display.display ();
}

/*********************************************
 * Calibrated light reading based on 10k
 * resistor across the solar cell, using
 * a standard photographic light meter for
 * the EV reading at 100 ASA.
 * 
 * EV LR (Light reading)
 * 4  45
 * 5  58
 * 6  76
 * 7  113
 * 8  176
 * 9  192
 * 10 205
 * 11 293
 * 12 330
 * 13 425
 * 14 457
 * 15 470
 * clearLcdChars (lineOneEraseFromPosition, 1);
 * Entering these numbers into 
 * https://www.easycalculation.com/analytical/least-squares-regression-line-equation.php
 * generated the formula to calculate EV based on 
 * the light reading.
 */
float evCalibrated (float lightReading) {
  float result;
  return result = 0.023*lightReading + 4.107;
}
