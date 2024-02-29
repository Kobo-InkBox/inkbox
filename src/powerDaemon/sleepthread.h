#ifndef SLEEPTHREAD_H
#define SLEEPTHREAD_H

#include <QObject>

class SleepThread : public QObject {
        Q_OBJECT
public:
    QString className = this->metaObject()->className();
    SleepThread();

public slots:
    void start();

signals:
    void startDialog();
    void stopDialog();
};

#endif // SLEEPTHREAD_H
