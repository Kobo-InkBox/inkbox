#ifndef DEVICEKOBOGLO_H
#define DEVICEKOBOGLO_H

#include "device.h"

class DeviceKoboGlo : public Device
{
public:
    DeviceKoboGlo();

    virtual QString getID() override {
        return "n613";
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
    virtual int getBrightness() override;
    virtual void setBrightness(int value) override;


    // SIZE
    virtual int getBaseDPI() override {
        return 175;
    };
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::MEDIUM;
    }
};

#endif // DEVICEKOBOGLO_H
