#ifndef DEVICEKOBOCLARAHD_H
#define DEVICEKOBOCLARAHD_H

#include "device.h"

class DeviceKoboClaraHD : public Device
{
public:
    DeviceKoboClaraHD();

    virtual QString getID() override {
        return "n249";
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
        return true;
    };
    virtual int getMaxWarmLightValue() override {
        return 100;
    };
    virtual int getWarmth() override;
    virtual void setWarmth(int value) override;

    // BRIGHTNESS
    virtual bool supportsBrightness() override {
        return true;
    };
    virtual void setBrightness(int value) override;

    // SIZE
    virtual int getBaseDPI() override {
        return 225;
    }
    virtual ScreenSize getScreenSize() override {
        return ScreenSize::LARGE;
    }

    //USB
    virtual bool isUSBPluggedIn() override;
    virtual void loadUSBMassStorageModule() override;

    // NETWORK
    virtual QString getNetworkInterfaceName() override {
        return "etho0";
    }
};

#endif // DEVICEKOBOCLARAHD_H
