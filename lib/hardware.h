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
 * get the variable resistor value
 *********************************************/
float getVariableResistorValue () {
  float variable;
  variable = analogRead (VARIABLE_RESISTOR);
  return variable;
}