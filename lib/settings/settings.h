#include <Arduino.h>

class settings {
    public:
        settings ();
        void setup (int);
        int getSetting (int, int);
        int getLastSetting (int);
        void setAperture (double apertureValue);
        int setDeltaAperture (int deltaAperture);
        double getAperture ();
        int setDeltaIso (int deltaIso);
        void setIso (double isoValue);
        double getIso ();
        void setshutter (String shutterValue);
        int setDeltaShutter (int deltaShutter);
        String getShutter ();
    private:
        const int minBrightness = 0;
        const int maxBrightness = 255;
        const int minIsoIDX = 0;
        const int maxIsoIDX = 13;
        const int minMeterStyle = 0;
        const int maxMeterStyle = 1;
        const int minShutterSpeedIDX = 0;
        const int maxShutterSpeedIDX = 22;
        const int minApertureIDX = 0;
        const int maxApertureIDX = 18;
        const int minPotValue = 0;
        const int maxPotValue = 1023;
        int apertureIDX;
        int apertureDeltaIDX;
        int isoIDX;
        int isoDeltaIDX;
        int shutterIDX;
        int shutterDeltaIDX;
        float aperture;
        float iso;
        String shutter;
        int numberOfSettings;
        int settingValue[3];
        int lastSettingValue[3];
// / number of values +2 aperture, shutter, iso
        int settingMaxValue[3] = {22+2, 14+2, 26+2};
        int settingMinValue[3] = {0, 0, 0};
        const double apertureTable[22] = {1.0,1.4, \
            2.0,2.8, \
            4.0,5.6, \
            8.0,11.0, \
            16.0,22.0, \
            32.0,45.0, \
            64.0,90.0, \
            128.0,181.0, \
            256.0,362.0, \
            512.0,724.0, \
            1024.0, 2048.0};
        const double isoTable[14] = {0.375,0.75, \
            1.5,3.0, \
            6.0,12.0, \
            25.0,50.0, \
            100.0,200.0, \
            400.0,800.0, \
            1600.0,3200.0};
        const String shutterTable[26] = {"1/1000","1/500", \
            "1/250","1/125", \
            "1/60","1/30", \
            "1/15","1/8", \
            "1/4","1/2", \
            "1 sec","2 sec", \
            "4 sec","8 sec", \
            "15 sec","30 sec", \
            "1 min","2 min", \
            "4 min","8 min", \
            "15 min","30 min", \
            "1 hr","2 hr", \
            "4 hr","8 hr"};
};