#include <Arduino.h>
// Note: installed these libraries with platformio library installer
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define upSwitch 8
#define downSwitch 9
#define lcdBackpanelLight 10 
#define solarPanelInput A0
#define changeVariablesPot A1

#define minLight 0
#define maxLight 360
#define minLightMapped 0
#define maxLightMapped 100

int getLightReading (float, float, float, float);

void setup() {
  pinMode (upSwitch, INPUT);
  pinMode (downSwitch, INPUT);
  pinMode (lcdBackpanelLight, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  float iso = 100;
  float fstop = 16;
  float shutter = 1/100;
  float evAdjust = 0.0;

  int ev;
  ev = getLightReading (iso, fstop, shutter, evAdjust);
  Serial.println ("Lignt reading is "+String(ev));
}

/*********************************************
 * get the light reading from the solar panel 
 *********************************************/
int getLightReading (float filmSpeed, float aperture, float shutterSpeed, float evCompensation){

  int lightRange = map (analogRead (solarPanelInput), minLight, maxLight, \
  minLightMapped, maxLightMapped);
  return lightRange; 
}
