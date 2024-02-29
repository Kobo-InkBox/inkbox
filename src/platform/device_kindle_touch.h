#ifndef DEVICEKINDLETOUCH_H
#define DEVICEKINDLETOUCH_H

#include "device.h"

class DeviceKindleTouch : public Device
{
public:
    DeviceKindleTouch();

    virtual QString getID() override {
        return "kt";
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
    virtual int getBatteryLevel() override;


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
        return ScreenSize::MEDIUM;
    }

    // USB
    virtual bool isUSBPluggedIn() override;
    virtual void loadUSBMassStorageModule() override;
    virtual QString getUSBMassStorageModule() override;

};

#endif // DEVICEKINDLETOUCH_H
