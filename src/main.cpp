#include <Arduino.h>
#include "../lib/hardware.h"
#include <SPI.h>
#include "../lib/LCD_Functions.h"
#include "../lib/GFX.h"
#include "../lib/settings/settings.h"
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

#define DELAY_TIME 200
#define LCD_CONTRAST 50
#define LCD_DEFAULT_BRIGHTNESS 127
#define numberOfScaleMarks 5
#define scaleRadius 32
#define markLineLength 4
#define vuStyleMeter 0
#define barStyleMeter 1
#define selectShutter 0
#define selectAperture 1

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
void meterChangeIndicator ( int, int);

settings meterSettings;

void setup () {

  SPI.setClockDivider(SPI_CLOCK_DIV16); // doesn't work with the Adafruit_GFX et.al. library
  setupHardware ();
  // pinMode (LCD_BACKLIGHT, OUTPUT);
  getScaleMarkCoordinates (needleBaseCoordinate.x, needleBaseCoordinate.y, \
    scaleRadius, numberOfScaleMarks, markLineLength);
  Serial.begin(9600);
  meterSettings.setup (getVariableResistorValue () );

  lcdBegin ();
  lcdBrightness (LCD_DEFAULT_BRIGHTNESS);

  setContrast (LCD_CONTRAST);
  clearDisplay (WHITE);
  updateDisplay ();

}

void loop() {
  // float iso = 100;
  // float fstop = 16;
  // float shutter = 1/100;
  double evAdjust = 0.0;
  float ev;
  float variableResistorValue;
  float test;
  int lightReading;
  int variableNumber = 0;

  // test setup of settings
  Serial.println("   ");
  Serial.println (String (getVariableResistorValue ()) );
  for (int i=0; i<3; i++){
    Serial.println (String (test = meterSettings.getLastSetting (i)) );
  }
  Serial.println("   ");

  String isoValue = String (meterSettings.getIso () );
  String apertureValue = String (meterSettings.getAperture () );
  String shutterValue = meterSettings.getShutter ();

  static int lastVariableChoice = 0;
  static unsigned long debounceTimeValue = 0;
  static int test_count = 0;

//  lightReading = getLightReading (isoValue, apertureValue, shutterValue, evAdjust);
  lightReading = getLightReading (evAdjust);
  ev = evCalibrated ( float (lightReading) ); // later this will be integrated into getLightReading
  variableResistorValue = getVariableResistorValue ();
  variableNumber = getVariableChoice (debounceTimeValue, lastVariableChoice);
  lastVariableChoice = variableNumber;
  testScreenDisplay (lightReading, ev, variableResistorValue, variableNumber, \
    isoValue, apertureValue, shutterValue);
  delay (DELAY_TIME);

/* VU meter line proof of concept (0-100%) meter needle movement
*/
  // for (int meter_percent = 0; meter_percent < 101 ; meter_percent++) {
  //   drawVUMeter ("f1024", "1/1000", 400, 0);
  //   updateVUMeter (meter_percent);
  //   delay (DELAY_TIME);
  //   clearDisplay (WHITE);
  // }

// float test = meterSettings.getIso();
  // Serial.println( "Aperture is "+String (meterSettings.getAperture () ) );
  // Serial.println( "Shutter is "+meterSettings.getShutter () );
  // Serial.println ( "ISO is "+String (meterSettings.getIso () ) );
  // delay (DELAY_TIME);
  // meterSettings.setIso(3200.0);
  // Serial.println ( String (meterSettings.getIso() ) );
  delay (DELAY_TIME);
}

/*********************************************
 * print message to lcd panel at coordinate
 *********************************************/
void lcdprint (position coordinate, String message) {
  unsigned int charNum = 0;
  while (message.charAt(charNum) != 0x00) {
    setChar ( message.charAt (charNum++), coordinate.x+(charNum*6), coordinate.y, BLACK);
  }

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

  const position fstopCoordinate = {54, 0}; //{54, 10};
  const position shutterSpeedCoordinate = { 0, 0}; // {48, 0};
  const position minusSignCoordinate = { 24, 40}; // all the way to the left {16, 40};
  const position plusSignCoordinate = { 50, 40}; // all the way to the left {42, 40};

// indicate selction  {shutter speed / aperture} with underline
  meterChangeIndicator  ( 0, 1);

// lables
  lcdprint (shutterSpeedCoordinate, shutter);
  lcdprint (fstopCoordinate, fstop.substring(0, 5));
  lcdprint (minusSignCoordinate, minusSign);
  lcdprint (plusSignCoordinate, plusSign);

// draw the centre of the needle
  fillCircle (needleBaseCoordinate.x, needleBaseCoordinate.y, needleBaseFillWidth, BLACK);

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
 * meter change (value) indicator
 *********************************************/
void meterChangeIndicator ( int meterStyle, int changeElement ) {
  int underscoreLengthShutter = 35;
  int underscoreLengthFstop = 27;
  int underscoreLength = underscoreLengthShutter;
  struct position vuShutter {0, 9};
  struct position barShutter {0, 15};
  struct position vuFstop {54, 9};
	struct position barFstop {0, 15};

  if ( meterStyle == vuStyleMeter )
  if ( changeElement == selectShutter ) {
    setLine (vuShutter.x, vuShutter.y, \
		vuShutter.x+underscoreLength, vuShutter.y, BLACK);
  }
  else {
    setLine (vuFstop.x, vuFstop.y, \
		vuFstop.x+underscoreLength, vuFstop.y, BLACK);
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