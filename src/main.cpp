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
#define upSwitch 8
#define downSwitch 9
#define lcdBackLight 10 
#define solarPanelInput A0
#define changeVariablesPot A1

// define some useful constants
#define delayTime 100


#define lcdContrast 50
#define initialLcdBrightness 127
#define lcdTextSize 1
#define lcdRotation 2

struct position {
  int x;
  int y;
};

// function prototypes
int getLightReading (float, float, float, float);
void lcdprint (position, String);

void setup() {
  pinMode (upSwitch, INPUT);
  pinMode (downSwitch, INPUT);
  pinMode (lcdBackLight, OUTPUT);
  Serial.begin(9600);

  display.begin(); // initialize display
  analogWrite (lcdBackLight, initialLcdBrightness);
  display.setContrast (lcdContrast);
  display.setTextSize (lcdTextSize);
  display.setRotation (lcdRotation);
  display.clearDisplay ();
}

void loop() {
  float iso = 100;
  float fstop = 16;
  float shutter = 1/100;
  float evAdjust = 0.0;
  int ev;
  position lineOnePosition = { 0, 0 };

  display.clearDisplay();
  ev = getLightReading (iso, fstop, shutter, evAdjust);
  lcdprint (lineOnePosition, "EV "+String(ev));
  delay (delayTime);

}

/*********************************************
 * get the light reading from the solar panel 
 *********************************************/
int getLightReading (float filmSpeed, float aperture, float shutterSpeed, float evCompensation){

  #define minLight 0
  #define maxLight 360
  #define minLightMapped 0
  #define maxLightMapped 100
  int light = analogRead (solarPanelInput);
  /* int light = map (analogRead (solarPanelInput), minLight, maxLight, \
   * minLightMapped, maxLightMapped);
   */
  return light; 
}

/*********************************************
 * print message to lcd panel at coordinate 
 *********************************************/
void lcdprint (position coordinate, String message) {
  display.setCursor (coordinate.x, coordinate.y);
  display.println (message);
  display.display ();
}
