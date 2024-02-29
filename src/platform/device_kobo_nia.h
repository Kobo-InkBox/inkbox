#ifndef DEVICEKOBONIA_H
#define DEVICEKOBONIA_H

#include "device.h"

class DeviceKoboNia : public Device
{
public:
    DeviceKoboNia();

    virtual QString getID() override {
        return "n306";
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
        return 175;
    }
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::MEDIUM;
    }

    //USB
    virtual void loadUSBMassStorageModule() override;
};

#endif // DEVICEKOBONIA_H
