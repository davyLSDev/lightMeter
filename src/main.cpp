#include <Arduino.h>
#include "../lib/hardware.h"
#include <SPI.h>                  // LCD_REFACTORING
#include "../lib/LCD_Functions.h" // LCD_REFACTORING
#include "../lib/GFX.h"
// Note: installed these libraries with platformio library installer
// LCD_REFACTORING #include <Adafruit_GFX.h>
// LCD_REFACTORING #include <Adafruit_PCD8544.h>

/* wiring of LCD panel to Arduino Uno
 * Arduino pin 13 - Serial clock out (SCLK)
 * Arduino pin 11 - Serial data out (DN)
 * Arduino pin 5 - Data/Command select (D/C)
 * Arduino pin 6 - LCD reset (RST)
 * Arduino pin 7 - LCD chip enable (SCE)
 */

/* define Arduino hardware pins used
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
// #define SOLAR_CELL_INPUT A0
// #define VARIABLE_RESISTOR A1

#define DELAY_TIME 200
#define LCD_CONTRAST 50
#define LCD_DEFAULT_BRIGHTNESS 127
#define numberOfScaleMarks 5
#define scaleRadius 32
#define markLineLength 4

struct position {
  int x;
  int y;
};

// maybe this is one reason for writing all this in c++ to use classes and members
static position markBottom[numberOfScaleMarks];
static position markTop[numberOfScaleMarks];
static const position needleBaseCoordinate = { 40, 47};

// function prototypes
//int getLightReading (float, float, float, float);
int getVariableChoice (unsigned long, int);
void lcdprint (position, String);
void testScreenDisplay (int, float, float, int, String, String, String);
void updateVUMeter (int);
float evCalibrated (float);
float getVariableResistorValue ();
void drawVUMeter (String, String, int, int);
void getScaleMarkCoordinates (int, int, int , int, int);
void scaleMarks (int, int, int, int, int);

void setup () {
  SPI.setClockDivider(SPI_CLOCK_DIV16); // doesn't work with the Adafruit_GFX et.al. library
  pinMode (UP_SWITCH, INPUT);
  pinMode (DOWN_SWITCH, INPUT);
  pinMode (LCD_BACKLIGHT, OUTPUT);
  getScaleMarkCoordinates (needleBaseCoordinate.x, needleBaseCoordinate.y, \
    scaleRadius, numberOfScaleMarks, markLineLength);
  Serial.begin(9600);

  lcdBegin ();
  analogWrite (LCD_BACKLIGHT, LCD_DEFAULT_BRIGHTNESS);
/*  display.setContrast (LCD_CONTRAST); // contrast doesn't work that well
  display.setTextSize (LCD_TEXT_SIZE); // consider re-writing the graphics functions
  display.setRotation (LCD_ROTATION);
  */
  setContrast (LCD_CONTRAST);
  clearDisplay (WHITE);
  updateDisplay ();
}

void loop() {
  float iso = 100;
  float fstop = 16;
  float shutter = 1/100;
  float evAdjust = 0.0;
  float ev;
  float variableResistorValue;
  int lightReading;
  int variableNumber = 0;
  String isoValue = "I:3200";
  String apertureValue = "f 1024";
  String shutterValue = "1/1000";

  static int lastVariableChoice = 0;
  static unsigned long debounceTimeValue = 0;
  static int test_count = 0;

//  clearDisplay (WHITE); // clear every loop either here or in an lcdUpdate function
  lightReading = getLightReading (iso, fstop, shutter, evAdjust);
  ev = evCalibrated ( float (lightReading) ); // later this will be integrated into getLightReading
  variableResistorValue = getVariableResistorValue ();
  variableNumber = getVariableChoice (debounceTimeValue, lastVariableChoice);
  lastVariableChoice = variableNumber;
/*  testScreenDisplay (lightReading, ev, variableResistorValue, variableNumber, \
    isoValue, apertureValue, shutterValue); */

/* VU meter line proof of concept (0-100%) meter needle movement
*/
  for (int meter_percent = 0; meter_percent < 101 ; meter_percent++) {
    drawVUMeter ("f 2.8", "1/250", 400, 0);
    updateVUMeter (meter_percent);
    delay (DELAY_TIME);
    clearDisplay (WHITE);
  }

}

// /*********************************************
//  * get the light reading from the solar panel
//  *********************************************/
// int getLightReading (float filmSpeed, float aperture, float shutterSpeed, float evCompensation){

//   #define MIN_LIGHT 0
//   #define MAX_LIGHT 360
//   #define MIN_EV 0
//   #define MAX_EV 100
//   int light = analogRead (SOLAR_CELL_INPUT);
//   return light;
//   // later use this: return evCalibrated ( light );
// }

/*********************************************
 * print message to lcd panel at coordinate
 *********************************************/
void lcdprint (position coordinate, String message) {
  unsigned int charNum = 0;
  while (message.charAt(charNum) != 0x00) {
    setChar ( message.charAt (charNum++), coordinate.x+(charNum*6), coordinate.y, BLACK);
  }

/*  display.setCursor (coordinate.x, coordinate.y);
  display.println (message);
  display.display ();
  */
}
/*********************************************
 * Calibrated light reading based on 10k
 * resistor across the solar cell, using
 * a standard photographic light meter for
 * the EV reading at 100 ASA.
 *
 * EV LR (Light reading)
 * 4  45
 * 5  58Begin
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
 *********************************************/
float evCalibrated (float lightReading) {
  float result;
  return result = 0.023*lightReading + 4.107;
}

// /*********************************************
//  * get the variable resistor value
//  *********************************************/
// float getVariableResistorValue () {
//   float variable;
//   variable = analogRead (VARIABLE_RESISTOR);
//   return variable;
// }

/*********************************************
 * fetch the variable choice
 ********************************************/
int getVariableChoice (unsigned long lastTime, int lastChoice){
/*  0 -> no changes (like the recal screen)
 * 1 -> brightness
 * Does nothing I can notice 2 -> contrast
 * 2 -> ISO
 * 3 -> VU style meter
 * 4 -> bargraph
 *
 *      One of the meter screens:
 * 5 -> shutter speed
 * 6 -> aperature
 *
 *      Recalibrate screen:
 * 7 -> recalibrate control
 */

  unsigned long timeNow = millis();
  const unsigned long debounceDelay = 50;

  int upSwitchState = digitalRead(UP_SWITCH);
  int downSwitchState = digitalRead(DOWN_SWITCH);
  int numberOfChoices = 8-1; // counting starts at zero

  static int lastUpSwitchState = 1;
  static int lastDownSwitchState = 1;

  if ( (timeNow - lastTime ) >= debounceDelay){
    lastTime = timeNow;
    if ( upSwitchState != lastUpSwitchState ) {
      lastUpSwitchState = upSwitchState;
      if ( upSwitchState == 0 ) {
        lastChoice++;
      }
    }

    if ( downSwitchState != lastDownSwitchState ) {
      if ( downSwitchState == 0 ) {
        lastChoice--;
      }
    }

    if ( lastChoice > numberOfChoices ) lastChoice = 0; // wraparound
    if ( lastChoice < 0 ) lastChoice = numberOfChoices;
  }

  return lastChoice;
}

/*********************************************
 * test screen display with simple data
 *********************************************/
void testScreenDisplay (int howMuchLight, float exposureValue, float potValue, int switchesState, \
  String iso, String aperture, String shutter) {

  position lineOnePosition = { 0, 0 };
  position lineTwoPosition = { 0, 10 };
  position lineThreePosition = { 0, 20 };
  position lineFourPosition = { 0, 30 };
  position isoPosition = { 48, 0 };
  position aperturePosition = { 0, 40 };
  position shutterPosition = { 48, 40 };

//  display.clearDisplay (); // how can this be avoided?
  lcdprint (lineOnePosition, "LR "+String (howMuchLight) );
  lcdprint (lineTwoPosition, "EV "+String(exposureValue) );
  lcdprint (lineThreePosition, "Pot "+String (potValue) );
  lcdprint (lineFourPosition, "SW state "+String (switchesState) );
  lcdprint (isoPosition, iso);
  lcdprint (aperturePosition, aperture);
  lcdprint (shutterPosition, shutter);
  updateDisplay();
}

/*********************************
 * draw VU style meter SIMPLIFIED
 *********************************/
void drawVUMeter (String fstop, String shutter, int iso, int changeVariable){
  const byte upperLeftArc = 0x1;
  const byte upperRightArc = 0x2;
//  byte lowerRightArc = 0x4;
//  byte lowerLeftArc = 0x8;
  const byte scaleArcs = upperLeftArc | upperRightArc;
  const int scaleBaseRadius = 6;
  const int needleBaseFillWidth = 2;

  const String minusSign = "-";
  const String plusSign = "+";

//  struct coordinate isoValueCoordinate = {0, 0};
//  struct coordinate changeLableCoordinate = {66, 40};
  const position fstopCoordinate = {48, 0}; //{54, 10};
  const position shutterSpeedCoordinate = { 0, 0}; // {48, 0};
  const position minusSignCoordinate = { 24, 40}; // all the way to the left {16, 40};
  const position plusSignCoordinate = { 50, 40}; // all the way to the left {42, 40};

  // display.clearDisplay ();

// lables
//   display.setCursor (fstopCoordinate.x, fstopCoordinate.y);
//   display.println (fstop.substring(0, 5));
//   display.setCursor (shutterSpeedCoordinate.x, shutterSpeedCoordinate.y);
//   display.println (shutter);

//   if (changeVariable !=0){
// //    display.setCursor(changeLableCoordinate.x, changeLableCoordinate.y);
// //    display.println(changeLable[changeVariable]);
//   }

//  display.setCursor(isoValueCoordinate.x, isoValueCoordinate.y);
//  display.println(String (iso));
  // display.setCursor (minusSignCoordinate.x, minusSignCoordinate.y);
  // display.println (minusSign);
  // display.setCursor (plusSignCoordinate.x, plusSignCoordinate.y);
  // display.println (plusSign);

  lcdprint (minusSignCoordinate, minusSign);
  lcdprint (plusSignCoordinate, plusSign);

// draw the centre of the needle
  // display.fillCircle (needleBaseCoordinate.x, needleBaseCoordinate.y, needleBaseFillWidth, BLACK);
  // display.display ();

// draw the meter's scale using circle segments
  drawCircleHelper (needleBaseCoordinate.x, needleBaseCoordinate.y, scaleBaseRadius, scaleArcs, BLACK);
  drawCircleHelper (needleBaseCoordinate.x, needleBaseCoordinate.y, scaleRadius, scaleArcs, BLACK);

// draw the scale marks
  scaleMarks (needleBaseCoordinate.x, needleBaseCoordinate.y, numberOfScaleMarks, scaleRadius, markLineLength);
}

/*********************
 * calculate the scale mark coordinates
 *********************/
void getScaleMarkCoordinates (int xCoordinate, int yCoordinate, int radius, int numberOfMarks, int markLength){
  int xTop;
  int yTop;
  int xBottom;
  int yBottom;
  int outsideRadius = radius + markLength;
  int markNumber=1;

  float Pi = 3.1415926;
  int angleForEachMark = 180/(1+numberOfMarks);
  int angleToPlaceMark = 90;

  while (markNumber <= numberOfMarks) {
    angleToPlaceMark = markNumber*angleForEachMark;
    float angle = angleToPlaceMark*Pi/180.0; // trig functions are in radians!
    markTop[markNumber-1].x = xCoordinate - int(outsideRadius*cos(angle));
    markTop[markNumber-1].y = yCoordinate - int(outsideRadius*abs(sin(angle)));
    markBottom[markNumber-1].x = xCoordinate - int(radius*cos(angle));
    markBottom[markNumber-1].y = yCoordinate - int(radius*abs(sin(angle)));
    markNumber++;
  }
}

/************************
 * draw the scale marks
 ***********************/
void scaleMarks (int xCoordinate, int yCoordinate, int numberOfMarks, int radius, int markLength){

  int markNumber=1;
  while (markNumber <= numberOfMarks) {
    setLine (markBottom[markNumber-1].x, markBottom[markNumber-1].y, markTop[markNumber-1].x, markTop[markNumber-1].y, BLACK);
    markNumber++;
  }
}

/*********************************************
 * update the VU style meter
 *********************************************/
void updateVUMeter (int meterValue){
  const float Pi = 3.1415926;
  const int needleRadius = 38;
  const position needleBaseCoordinate = {40, 47};
  int xTip;
  int yTip;

  float angle = meterValue*Pi/100.0; // trig functions are in radians!

  xTip = needleBaseCoordinate.x - int(needleRadius*cos(angle));
  yTip = needleBaseCoordinate.y - int(needleRadius*abs(sin(angle)));

  setLine (needleBaseCoordinate.x, needleBaseCoordinate.y, xTip, yTip, BLACK);
  updateDisplay ();
}
