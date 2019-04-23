/* define Arduino hardware pins used
 *
 * Arduino pin  8  -> LH switch (up)
 * Arduino pin  9  -> RH switch (down)
 * Arduino pin  10 -> pwm pin for lcd brightness
 * Arduino pin  A0 -> solar panel
 * Arduino pin  A1 -> pot
 */
#define SOLAR_CELL_INPUT A0
#define VARIABLE_RESISTOR A1
#define UP_SWITCH 8
#define DOWN_SWITCH 9
#define LCD_BACKLIGHT 10

/*********************************************
 * set the lcd backlight brightness
 *********************************************/
void setupHardware (){
  pinMode (UP_SWITCH, INPUT);
  pinMode (DOWN_SWITCH, INPUT);
  pinMode (LCD_BACKLIGHT, OUTPUT);
}

/*********************************************
 * set the lcd backlight brightness
 *********************************************/
/*  display.setContrast (LCD_CONTRAST); // contrast doesn't work that well
  display.setTextSize (LCD_TEXT_SIZE); // consider re-writing the graphics functions
  display.setRotation (LCD_ROTATION);
  */
void lcdBrightness (int brightness){
  analogWrite (LCD_BACKLIGHT, brightness);
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