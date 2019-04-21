#define SOLAR_CELL_INPUT A0
#define VARIABLE_RESISTOR A1

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