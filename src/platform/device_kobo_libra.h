#ifndef DEVICEKOBOLIBRA_H
#define DEVICEKOBOLIBRA_H

#include "device.h"

class DeviceKoboLibra : public Device
{
public:
    DeviceKoboLibra();

    virtual QString getID() override {
        return "n873";
    }

    virtual bool hasNavButtons() override {
        return true;
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
        return true;
    };
    virtual int getMaxWarmLightValue() override {
        return 10;
    };
    virtual int getWarmth() override;
    virtual void setWarmth(int value) override;

    // BRIGHTNESS
    virtual bool supportsBrightness() override {
        return true;
    };

    // SIZE
    virtual int getBaseDPI() override {
        return 250;
    }
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::VERY_LARGE;
    }

    //USB
    virtual void loadUSBMassStorageModule() override;
};

#endif // DEVICEKOBOLIBRA_H
