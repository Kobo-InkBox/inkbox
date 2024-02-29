#include "device_kobo_glo.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

#include <QFile>
#include <QTextStream>

DeviceKoboGlo::DeviceKoboGlo()
{

}

int DeviceKoboGlo::getBrightness() {
    return readFile(".config/03-brightness/config").toInt();
}

void DeviceKoboGlo::setBrightness(int value) {
    int fd;
    if((fd = open("/dev/ntx_io", O_RDWR)) == -1) {
        fprintf(stderr, "Error opening ntx_io device while setting brightness.\n");
        return;
    }
    ioctl(fd, 241, value);
    close(fd);

    // also store it somewhere so we can load it when we need it for getBrightness
    std::ofstream fhandler;
    fhandler.open(".config/03-brightness/config");
    fhandler << value;
    fhandler.close();
}
