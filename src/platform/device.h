#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class Device
{
private:
    static Device* instance;
    QString powerSupplyPath;

    QString getPowerSupplyPath();

protected:
    Device();

    void writeFile(QString path, QString content);
    QString readFile(QString path);

public:
    virtual ~Device() = default;

    static void setup(QString& platformName);
    /*

            if(global::deviceID == "n705\n") {
                global::isN705 = true;
            }
            else if(global::deviceID == "n905\n") {
                global::isN905C = true;
            }
            else if(global::deviceID == "n613\n") {
                global::isN613 = true;
            }
            else if(global::deviceID == "n873\n") {
                global::isN873 = true;
            }
            else if(global::deviceID == "n236\n") {
                global::isN236 = true;
            }
            else if(global::deviceID == "n437\n") {
                global::isN437 = true;
            }
            else if(global::deviceID == "n306\n") {
                global::isN306 = true;
            }
            ^else if(global::deviceID == "n249\n") {
                global::isN249 = true;
            }
            else if(global::deviceID == "kt\n") {
                global::isKT = true;
            }
            else {
                ;
            }

*/

    static Device* getSingleton();

    virtual QString getID() = 0;

    struct SysInfo {
        QString uid;

        QString kernelVersion;
        QString kernelBuild;
        QString kernelCommit;

        QString inkOSVersion;
        QString inkGitVersion;

        QString ipAddress;
    };
    virtual SysInfo getSystemInfo();

    virtual bool hasNavButtons() = 0;

    virtual bool supportsWifi() = 0;

    // BATTERY stuff

    virtual bool hasBattery() = 0;
    virtual int getBatteryLevel();
    // Generic Battery Utility functions
    bool isBatteryCritical() {
        return getBatteryLevel() < 5;
    }
    bool isBatteryLow() {
        return getBatteryLevel() < 15;
    }

    // only true for isN249 and isN873
    virtual bool supportsWarmLight() = 0;
    // 100 for N249 and 10 for N873
    virtual int getMaxWarmLightValue() = 0;
    virtual int getWarmth() = 0;
    virtual void setWarmth(int value) = 0;

    virtual bool supportsBrightness()=0;

    virtual int getBrightness();
    virtual void setBrightness(int value);

    virtual bool supportsNightmode();

    virtual int getBaseDPI();

    enum ScreenSize {
        SMALL,  // Kobo Mini (705)
        MEDIUM,
        LARGE,  // Kobo Glo HD (437), 249
        VERY_LARGE  // Kobo Libra (873)
    };
    virtual ScreenSize getScreenSize() = 0;

    // USB Mass Storage Stuff
    virtual bool isUSBPluggedIn();
    virtual void loadUSBMassStorageModule();
    virtual QString getUSBMassStorageModule();

    // NETWORK
    virtual QString getNetworkInterfaceName();

    // PATHS
    virtual const QString getStyleSheetPath();

};

#endif // PLATFORM_H
