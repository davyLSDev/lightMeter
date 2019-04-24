#include "settings.h"
using namespace std;

settings::settings()
{
    apertureIDX = 8;    // 16.0
    isoIDX = 8;         //8 100.0
    shutterIDX = 3;     //3 "1/125"
}

void settings::setIso (double isoValue)
{
    iso = isoValue;
}

double settings::getAperture ()
{
    return aperture = apertureTable[apertureIDX];
}

double settings::getIso ()
{
    return iso = isoTable[isoIDX];
}

String settings::getShutter ()
{
    return shutter = shutterTable[shutterIDX];
}