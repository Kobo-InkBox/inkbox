#ifndef DEVICEKOBOGLOHD_H
#define DEVICEKOBOGLOHD_H

#include "device.h"

class DeviceKoboGloHD : public Device
{
public:
    DeviceKoboGloHD();

    virtual QString getID() override {
        return "n437";
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
        return 225;
    }
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::LARGE;
    }

    //NETWORK
    virtual QString getNetworkInterfaceName() override {
        return "eth0";
    }
};

#endif // DEVICEKOBOGLOHD_H
