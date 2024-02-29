#ifndef DEVICEDESKTOP_H
#define DEVICEDESKTOP_H

#include "device.h"

class DeviceDesktop : public Device
{
public:
    DeviceDesktop();

    virtual QString getID() override {
        return "desktop";
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
    virtual int getBaseDPI() override {
        return 125;
    }
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::VERY_LARGE;
    }
};

#endif // DEVICEDESKTOP_H
