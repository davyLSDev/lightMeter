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
#define VARIABLE_RESISTOR A1

// define some useful constants
#define DELAY_TIME 200
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
int getVariableChoice (unsigned long, int);
void lcdprint (position, String);
void testScreenDisplay (int, float, float, int);
float evCalibrated (float);
float getVariableResistorValue ();

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
  float ev;
  float variableResistorValue;
  int lightReading;
  int variableNumber = 0;

  static int lastVariableChoice = 0;
  static unsigned long debounceTimeValue = 0;
  // static int test =0;
  
  lightReading = getLightReading (iso, fstop, shutter, evAdjust);
  ev = evCalibrated ( float (lightReading) ); // later this will be integrated into getLightReading
  variableResistorValue = getVariableResistorValue ();
  variableNumber = getVariableChoice (debounceTimeValue, lastVariableChoice);
  lastVariableChoice = variableNumber;
  testScreenDisplay (lightReading, ev, variableResistorValue, variableNumber);

  // to prove a point with "static" Serial.println ("number is "+String(test++));
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
 *********************************************/
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
 *********************************************/
float evCalibrated (float lightReading) {
  float result;
  return result = 0.023*lightReading + 4.107;
}

/*********************************************
 * get the variable resistor value
 *********************************************/
float getVariableResistorValue () {
  float variable;
  variable = analogRead (VARIABLE_RESISTOR);
  return variable;
}

/*********************************************
 * fetch the variable choice
/*********************************************
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
void testScreenDisplay (int howMuchLight, float exposureValue, float potValue, int switchesState){
  position lineOnePosition = { 0, 0 };
  position lineTwoPosition = { 0, 10 };
  position lineThreePosition = { 0, 20 };
  position lineFourPosition = { 0, 30 };

  display.clearDisplay (); // how can this be avoided?
  lcdprint (lineOnePosition, "LR "+String (howMuchLight) );
  lcdprint (lineTwoPosition, "EV "+String(exposureValue) );
  lcdprint (lineThreePosition, "Pot "+String (potValue) );
  lcdprint (lineFourPosition, "SW state "+String (switchesState) );
}
