#ifndef WIFIDIALOG_H
#define WIFIDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "generaldialog.h"
#include "wifilogger.h"

namespace Ui {
class WifiDialog;
}

class WifiDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit WifiDialog(QWidget *parent = nullptr);
    ~WifiDialog();
    global::wifi::wifiNetworkData connectedNetworkDataParent;
    bool connectedNetworkDataParentSet = false;

private:
    Ui::WifiDialog *ui;
    bool wifiButtonEnabled = false;
    bool scannedAtLeastOnce = false;

    // Variables for refreshWait() and network refresh in general
    int elapsedSeconds = 0;
    QFile fullList =  QFile("/external_root/run/wifi_list_full");
    QFile formattedList = QFile("/external_root/run/wifi_list_format");

    // Used by watcher
    bool forceRefresh = false;
    int relaunchMs = 300;
    bool refreshFromWatcher = false;
    bool unlockCheckBox = false;
    bool scanInProgress = false;

    bool isToggleRunning = false;
    bool ignoreCheckBoxCall = false;

    bool secondScanTry = false;

public slots:
    void launchRefresh();
    void refreshNetworksList();
    void showToastSlot(QString message);
    void refreshScreenSlot();

    // Shows status of Wi-Fi processes, like reconnection and others. Also manages refreshing the networks list after connection
    void watcher();

signals:
    void refreshScreen();
    void updateWifiIconSig(int mode);
    void showToast(QString messageToDisplay);
    void killNetworkWidgets();

private slots:
    void on_refreshBtn_clicked();
    void on_wifiCheckBox_stateChanged(int arg1);
    void turnOnWifi();
    void turnOffWifi();
    void on_logBtn_clicked();

    // This function is a more clever sleep(1), non-blocking
    void refreshWait();

    void setStatusText(QString message);
    void on_stopBtn_clicked();
    void on_returnBtn_clicked();
    void waitToScan();
};

#endif // WIFIDIALOG_H
