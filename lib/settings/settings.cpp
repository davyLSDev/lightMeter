#include "settings.h"
using namespace std;

settings::settings()
{
    // Defaults . . .
    aperture = 16;
    iso = 100.0;
    shutter = "1/125";
    numberOfSettings = 3; // aperture, iso, shutter
    // later add: display brightness, choice of meter style
}

void settings::setup (int potentiometerReading)
{
    for ( int i = 0; i < numberOfSettings; i++ )
    {
        settingValue[i] = lastSettingValue[i] = map ( potentiometerReading, \
        minPotValue, maxPotValue, \
        settingMinValue[i], settingMaxValue[i]+settingPadding );
    }
    apertureIDX = 0;
    isoIDX = 1;
    shutterIDX = 2;
}

int settings::getDeltaSetting (int settingNumber, int potReading)
{
    int currentSetting = this->getSetting (settingNumber, potReading);
    int lastSetting = this->getLastSetting (settingNumber);
    int deltaSetting = currentSetting-lastSetting;
    if ( deltaSetting !=0 )
    {
        lastSettingValue[settingNumber] = currentSetting;
        settingIDX[settingNumber] = lastSetting + deltaSetting;
// Don't let array indices go out of bounds!
        if ( settingIDX[settingNumber] > settingMaxValue[settingNumber] )
        {
            settingIDX[settingNumber] = settingMaxValue[settingNumber];
        }
        if ( settingIDX[settingNumber] < settingMinValue[settingNumber] )
        {
            settingIDX[settingNumber] = settingMinValue[settingNumber];
        }
    }
    return deltaSetting;
}

int settings::getSetting (int settingNumber, int potReading)
{
    return settingValue[settingNumber] = map ( potReading, \
        minPotValue, maxPotValue, \
        settingMinValue[settingNumber], settingMaxValue[settingNumber] );
}

int settings::getLastSetting (int settingNumber)
{
    return lastSettingValue[settingNumber];
}

void settings::setIso (double isoValue)
{
    iso = isoValue;
}

double settings::getAperture ()
{
    return aperture = apertureTable[settingIDX[apertureIDX]];
}

double settings::getIso ()
{
    return iso = isoTable[settingIDX[isoIDX]];
}

String settings::getShutter ()
{
    return shutter = shutterTable[settingIDX[shutterIDX]];
}