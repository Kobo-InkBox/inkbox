#include "device_kobo_nia.h"

#include <QProcess>

DeviceKoboNia::DeviceKoboNia()
{

}


void DeviceKoboNia::loadUSBMassStorageModule() {
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/fs/configfs/configfs.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/libcomposite.ko");
    QProcess::execute("insmod", QStringList() << "/external_root/lib/modules/drivers/usb/gadget/function/usb_f_mass_storage.ko");

}
