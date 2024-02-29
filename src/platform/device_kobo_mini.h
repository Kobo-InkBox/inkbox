#ifndef DEVICEKOBOMINI_H
#define DEVICEKOBOMINI_H

#include "device.h"

class DeviceKoboMini : public Device
{
public:
    DeviceKoboMini();

    virtual QString getID() override {
        return "n705";
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
        return ScreenSize::SMALL;
    }
};

#endif // DEVICEKOBOMINI_H
