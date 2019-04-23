#include <Arduino.h>

class settings {
    public:
        settings();
        void setIso (float isoValue);
        float getIso ();
    private:
        float iso;
};