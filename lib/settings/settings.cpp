#include "settings.h"
using namespace std;

settings::settings()
{
    float iso = 0.0;
}

void settings::setIso (float isoValue)
{
    iso = isoValue;
}

float settings::getIso ()
{
    return iso;
}