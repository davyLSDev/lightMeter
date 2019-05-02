#include "settings.h"
using namespace std;

settings::settings()
{
    // Defaults . . .
    apertureIDX = 8;
    apertureDeltaIDX = 0;
    isoIDX = 8;
    isoDeltaIDX = 0;
    shutterIDX = 3;
    shutterDeltaIDX = 0;
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
        settingMinValue[i], settingMaxValue[i] );
        // lastSettingValue[i] = 300.0 ; // potentiometerReading;
        // settingValue[i] = potentiometerReading;
    }
    apertureDeltaIDX = 0;
    isoDeltaIDX = 0;
    shutterDeltaIDX = 0;
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
    // return aperture = apertureTable[apertureIDX];
    return aperture = apertureTable[settingIDX[0]];
}

double settings::getIso ()
{
    return iso = isoTable[isoIDX];
}

String settings::getShutter ()
{
    return shutter = shutterTable[shutterIDX];
}