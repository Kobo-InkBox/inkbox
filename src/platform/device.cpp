#include "device.h"
#include "functions.h"

#include <QDirIterator>

#include "device_desktop.h"
#include "device_kindle_touch.h"
#include "device_kobo_aura_edition2.h"
#include "device_kobo_clarahd.h"
#include "device_kobo_glo.h"
#include "device_kobo_glohd.h"
#include "device_kobo_libra.h"
#include "device_kobo_mini.h"
#include "device_kobo_nia.h"
#include "device_kobo_touch.h"


Device* Device::instance = nullptr;

Device::Device()
{    
    powerSupplyPath = getPowerSupplyPath();
}

Device* Device::getSingleton() {
    return instance;
}




void Device::setup(QString &platformName){
    QString name = platformName.trimmed();
    Device* temp;

#define SETUP_DEVICE(x) {       \
    temp = new x();             \
    if(temp->getID() == name) { \
        Device::instance = temp;\
        log("Setting up Device "+Device::instance->getID()+".", "Device::setup"); \
        return;                 \
    } else {                    \
        delete temp;            \
    }                           \
}

    SETUP_DEVICE(DeviceKindleTouch)

    SETUP_DEVICE(DeviceKoboAuraEdition2)
    SETUP_DEVICE(DeviceKoboClaraHD)
    SETUP_DEVICE(DeviceKoboGlo)
    SETUP_DEVICE(DeviceKoboGloHD)
    SETUP_DEVICE(DeviceKoboLibra)
    SETUP_DEVICE(DeviceKoboMini)
    SETUP_DEVICE(DeviceKoboNia)
    SETUP_DEVICE(DeviceKoboTouch)

    // Fallback if none of the devices above worked
    Device::instance = new DeviceDesktop();

    log("Setting up Device "+Device::instance->getID()+".", "Device::setup");
    fprintf(stderr, "Device has fallen back to desktop.\n");
}

void Device::writeFile(QString path, QString content) {
    QFile file(path);
    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << content;
    } else {
        fprintf(stderr, ("Failed to write to file: "));
    }
}

QString Device::readFile(QString path) {
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QTextStream in (&file);
    QString content = in.readAll();
    file.close();

    return content.trimmed();
}

QString Device::getPowerSupplyPath() {
    // get the path to the battery so we can check the charge latter
    QString result;
    QDirIterator supplies("/sys/class/power_supply", QDirIterator::NoIteratorFlags);
    while(supplies.hasNext()) {
        QString supply = supplies.filePath();

        // Badly-classified device by some ntx kernels, ignore
        if (supplies.fileName() == "mc13892_charger") {
            continue;
        }

        if(QFile::exists(supply + "/type")) {
            QString content = readFile(supply + "/type");
            if(content == "Battery"){
                result = supply;
                break;
            }

        }

        supplies.next();
    }
    return result;
}


const QString Device::getStyleSheetPath() {
    return "/mnt/onboard/.adds/inkbox/";
}

Device::SysInfo Device::getSystemInfo() {
    // get UID
    QString prog ("dd");
    QStringList args;
    args << "if=/dev/mmcblk0" << "bs=512" << "skip=1" << "count=1" << "status=none";
    QProcess *proc = new QProcess();
    proc->start(prog, args);
    proc->waitForFinished();

    QString deviceUID = proc->readAllStandardOutput();
    deviceUID = deviceUID.left(256);
    proc->deleteLater();

    // get Kernel Version
    prog = "uname";
    args = QStringList();
    args << "-r";
    proc = new QProcess();
    proc->start(prog, args);
    proc->waitForFinished();

    QString kernelVersion = proc->readAllStandardOutput();
    kernelVersion = kernelVersion.trimmed();

    proc->deleteLater();

    setDefaultWorkDir();
    writeFile("/external_root/run/initrd-fifo", "get_kernel_build_id\n");
    QThread::msleep(100);
    writeFile("/external_root/run/initrd-fifo", "get_kernel_commit\n");
    QThread::msleep(100);

    QString kernelBuildID = readFile("/external_root/run/kernel_build_id").trimmed();

    QString kernelCommit = readFile("/external_root/run/kernel_commit").trimmed();


    SysInfo info = {
        .uid= deviceUID,
        .kernelVersion= kernelVersion,
        .kernelBuild= kernelBuildID,
        .kernelCommit= kernelCommit,

        .inkOSVersion= readFile("/external_root/opt/isa/version"),
        .inkGitVersion = GIT_VERSION,

        .ipAddress = getConnectionInformation()
    };

    return info;
}


bool Device::supportsWifi() {
    return checkconfig("/run/wifi_able");
}

int Device::getBatteryLevel() {
    int value = 100;
    if(QFile::exists("/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/capacity")) {
        value = readFile("/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/capacity").toInt();
    } else if(QFile::exists(powerSupplyPath + "/capacity")) {
        value = readFile(powerSupplyPath + "/capacity").toInt();
    }

    return value;
}

int Device::getBrightness() {
    if(QFile::exists("/var/run/brightness")) {
        return readFile("/var/run/brightness").toInt();
    }
    else {
        return 0;
    }
}

void Device::setBrightness(int value) {
    if(QFile::exists("/var/run/brightness")) {
        std::ofstream fhandler;
        fhandler.open("/var/run/brightness");
        fhandler << value;
        fhandler.close();
    } else if(QFile::exists("/var/run/brightness_write")) {
        // this is for N249 devices. Overwrite this virtual method for that platform instead
        std::ofstream fhandler;
        fhandler.open("/var/run/brightness_write");
        fhandler << value;
        fhandler.close();
    }
}

bool Device::supportsNightmode() {
    return true;
}

int Device::getBaseDPI() {
    return 125;
}

bool Device::isUSBPluggedIn() {
    // Thanks to https://github.com/koreader/KoboUSBMS/blob/2efdf9d920c68752b2933f21c664dc1afb28fc2e/usbms.c#L148-L158
    int ntxfd;
    if((ntxfd = open("/dev/ntx_io", O_RDWR)) == -1) {
            fprintf(stderr, "Error opening ntx_io device\n");
            return false;
    }
    unsigned long ptr = 0U;
    ioctl(ntxfd, 108, &ptr);
    close(ntxfd);
    return !!ptr;
}

void Device::loadUSBMassStorageModule() {

}

QString Device::getUSBMassStorageModule() {
    return "/external_root/lib/modules/g_mass_storage.ko";
}

QString Device::getNetworkInterfaceName() {
    return "wlan0";
}
