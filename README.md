# lightMeter

This light meter is envisioned for being used with a 4x5 camera.

## About

The intial code commit was based on prior art in *solar_lcdMeter* sketch found in my [arduino-testing](https://github.com/davyLSDev/arduino-testing) repo.

## Hardware required

* Arduino Uno (if not this exact model, you might need to adapt this software a bit)
* a solar panel (you might need to tweak the calculations for the meters)
* a potentiometer
* a couple of pushbutton switches
* an Nokia 5110 lcd panel

## Roadmap

1. Make the buttons select either the shutter speed, or aperture to adjust on the VU meter
2. Add in the start up screen with adjustment for lcd brightness
3. Make the buttons select between the screens
4. Calibrate the light reading to correspond to 0, +/-{1-3}, where 3 is right at the bottom of the meter on both sides.
5. Make a bargraph style meter display, enable buttons to select this display
6. Add in the schematic diagram to the project files
7. Program a discreet ATMega chip
8. Build the circuit on a board
9. Make the 4x5 "film holder" case