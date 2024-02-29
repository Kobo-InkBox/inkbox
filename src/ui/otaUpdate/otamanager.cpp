#include "otamanager.h"
#include "ui_otamanager.h"
#include "functions.h"

#include <QTimer>
#include <QDebug>
#include <QDateTime>

OTAUpdateManagerWidget::OTAUpdateManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OTAUpdateManagerWidget)
{
    ui->setupUi(this);
    QThread::msleep(500);
    if(global::otaUpdate::downloadOta == false) {
        log("Checking for available OTA update", className);
        writeFile("/opt/ibxd", "ota_update_check\n");
        QTimer * otaCheckTimer = new QTimer(this);
        otaCheckTimer->setInterval(100);
        connect(otaCheckTimer, &QTimer::timeout, [&]() {
            if(QFile::exists("/run/can_ota_update") == true) {
                if(checkconfig("/run/can_ota_update") == true) {
                    log("OTA update is available", className);
                    emit canOtaUpdate(true);
                }
                else {
                    log("No OTA update available", className);
                    emit canOtaUpdate(false);
                }
                unsigned long currentEpoch = QDateTime::currentSecsSinceEpoch();
                writeFile("/external_root/opt/storage/update/last_sync", QString::number(currentEpoch));
                QFile::remove("/run/can_ota_update");
                OTAUpdateManagerWidget::close();
            }
        } );
        otaCheckTimer->start();
    }
    else {
        log("Downloading OTA update", className);
        QFile::remove("/run/can_install_ota_update");
        writeFile("/opt/ibxd", "ota_update_download\n");
        QTimer * otaDownloadTimer = new QTimer(this);
        otaDownloadTimer->setInterval(100);
        connect(otaDownloadTimer, &QTimer::timeout, [&]() {
            if(QFile::exists("/run/can_install_ota_update") == true) {
                if(checkconfig("/run/can_install_ota_update") == true) {
                    log("Download succeeded", className);
                    emit downloadedOtaUpdate(true);
                    global::otaUpdate::downloadOta = false;
                }
                else {
                    log("Download failed", className);
                    emit downloadedOtaUpdate(false);
                    global::otaUpdate::downloadOta = false;
                }
                QFile::remove("/external_root/opt/storage/update/last_sync");
                OTAUpdateManagerWidget::close();
            }
        } );
        otaDownloadTimer->start();
    }
}

OTAUpdateManagerWidget::~OTAUpdateManagerWidget()
{
    delete ui;
}
