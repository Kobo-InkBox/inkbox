#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QApplication>
#include "functions.h"

class Telemetry : public QObject
{
    Q_OBJECT
public:
    explicit Telemetry(QObject *parent = nullptr);
    QString className = this->metaObject()->className();
    QString message;
    QString deviceUID;
    QString systemVersion;
    QString device;
    bool deviceRooted;
    bool developerKeyInstalled;
    bool requestSuccessful;

private:
    QJsonObject collectDeviceInformation();
    bool sendDeviceInformation(QJsonObject data);

private slots:
    void telemetrySlot();

signals:
    void showToast(QString message);
};

#endif // TELEMETRY_H
