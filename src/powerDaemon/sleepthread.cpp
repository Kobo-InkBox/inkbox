#include "sleepthread.h"
#include "functions.h"

#include <stdlib.h>

sleepThread::sleepThread() {}

void sleepThread::start()
{
    log("Sleep pipe thread active", className);
    QDir pipeDirPath = QDir("/dev/ipd");
    QFile pipePath = QFile("/dev/ipd/fifo");
    while(true) {
        QThread::sleep(1);
        if(pipePath.exists() == true and pipeDirPath.exists() == true) {
            log("Looking for messages in pipe", className);
            char * pipe = "/dev/ipd/fifo";
            int fd = ::open(pipe, O_RDONLY);
            char * readchar[5];
            ::read(fd, readchar, 5);

            QString result = QString::fromUtf8((const char *)readchar, 5);

            log("Received from pipe: " + result, className);

            ::close(fd);

            if(result == "start") {
                log("Emiting signal to show sleepDialog", className);
                emit startDialog();
            }
            else if (result == "stop0") {
                log("Emiting signal to hide sleepDialog", className);
                emit stopDialog();
            }
            else {
               log("Something went wrong", className);
            }
        }
    }
}
