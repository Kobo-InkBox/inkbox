#include "device_kobo_libra.h"
#include <QProcess>

DeviceKoboLibra::DeviceKoboLibra()
{

}

int DeviceKoboLibra::getWarmth() {
    QString sysfsWarmthPath = "/sys/class/backlight/lm3630a_led/color";
    int warmthValue;
    QString warmthConfig = readFile(sysfsWarmthPath);
    warmthValue = warmthConfig.toInt();
    warmthValue = 10 - warmthValue;

    return warmthValue;
}

void DeviceKoboLibra::setWarmth(int value) {
    QString sysfsWarmthPath;
    QString warmthValueStr;
    // Value 0 gives a warmer lighting than value 10
    value = 10 - value;
    warmthValueStr = QString::number(value);
    sysfsWarmthPath = "/sys/class/backlight/lm3630a_led/color";

    writeFile(sysfsWarmthPath, warmthValueStr);
}

void DeviceKoboLibra::loadUSBMassStorageModule() {
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/fs/configfs/configfs.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/libcomposite.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/function/usb_f_mass_storage.ko");

}
