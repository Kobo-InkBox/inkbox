#include "device_kindle_touch.h"

#include <QFile>
#include <QProcess>

DeviceKindleTouch::DeviceKindleTouch()
{

}

int DeviceKindleTouch::getBatteryLevel() {
    if(QFile::exists("/sys/devices/system/yoshi_battery/yoshi_battery0/battery_capacity")) {
        return readFile("/sys/devices/system/yoshi_battery/yoshi_battery0/battery_capacity").toInt();

    }
    return 100;
}

bool DeviceKindleTouch::isUSBPluggedIn() {
    return readFile("/sys/devices/system/yoshi_battery/yoshi_battery0/battery_status").toInt() == 1;
}

void DeviceKindleTouch::loadUSBMassStorageModule() {
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/2.6.35-inkbox/kernel/drivers/usb/gadget/arcotg_udc.ko");
}

QString DeviceKindleTouch::getUSBMassStorageModule() {
    return "/external_root/lib/modules/2.6.35-inkbox/kernel/drivers/usb/gadget/g_file_storage.ko";
}
