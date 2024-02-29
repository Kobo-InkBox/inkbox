#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QObject>
#include <sys/un.h>
#include <QTimer>

class AudioThread : public QObject
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    AudioThread();
    void sendInfo(QString message);
    int sockfd;
    struct sockaddr_un addr;
    int res;
    bool monitorProgress = false;
public slots:
    void start();
    void audioProgress();

private:
};

#endif // AUDIOTHREAD_H
