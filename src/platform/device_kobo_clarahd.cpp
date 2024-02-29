#include "device_kobo_clarahd.h"

#include <QFile>
#include <QProcess>

DeviceKoboClaraHD::DeviceKoboClaraHD()
{

}

int DeviceKoboClaraHD::getWarmth() {
    QString sysfsWarmthPath = "/sys/class/backlight/backlight_warm/actual_brightness";
    int warmthValue;

    QString warmthConfig = readFile(sysfsWarmthPath);
    warmthValue = warmthConfig.toInt();
    return warmthValue;
}

void DeviceKoboClaraHD::setWarmth(int value) {
    QString sysfsWarmthPath = "/sys/class/backlight/backlight_warm/brightness";
    QString warmthValueStr =QString::number(value);

    writeFile(sysfsWarmthPath, warmthValueStr);
}

void DeviceKoboClaraHD::setBrightness(int value) {
    if(QFile::exists("/var/run/brightness_write")) {
        writeFile("/var/run/brightness_write", QString::number(value));
    }
}


//USB
bool DeviceKoboClaraHD::isUSBPluggedIn() {
    return (readFile("/sys/class/power_supply/rn5t618-battery/status") != "Discharging\n");
}

void DeviceKoboClaraHD::loadUSBMassStorageModule() {
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/fs/configfs/configfs.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/libcomposite.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/function/usb_f_mass_storage.ko");
}
