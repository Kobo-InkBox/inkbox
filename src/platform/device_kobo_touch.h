#ifndef DEVICEKOBOTOUCH_H
#define DEVICEKOBOTOUCH_H

#include "device.h"

class DeviceKoboTouch : public Device
{
public:
    DeviceKoboTouch();



    virtual QString getID() override {
        return "n905";
    }

    virtual bool hasNavButtons() override {
        return false;
    }

    virtual bool supportsWifi() override {
        return true;
    }

    // BATTERY
    virtual bool hasBattery() override {
        return true;
    }

    // SCREEN
    // WARM LIGHT
    virtual bool supportsWarmLight() override {
        return false;
    };
    virtual int getMaxWarmLightValue() override {
        return 0;
    };
    virtual int getWarmth() override {return 0;};
    virtual void setWarmth(int value) override {};

    // BRIGHTNESS
    virtual bool supportsBrightness() override {
        return true;
    };

    // SIZE
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::MEDIUM;
    }

};

#endif // DEVICEKOBOTOUCH_H
