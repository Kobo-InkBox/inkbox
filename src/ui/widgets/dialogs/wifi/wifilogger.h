#ifndef WIFILOGGER_H
#define WIFILOGGER_H

#include <QDialog>
#include "functions.h"

namespace Ui {
class WifiLoggerDialog;
}

class WifiLoggerDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit WifiLoggerDialog(QWidget *parent = nullptr);
    ~WifiLoggerDialog();
    global::wifi::wifiNetworkData connectedNetworkData;
    // To the above value
    bool isThereData = false;

private:
    Ui::WifiLoggerDialog *ui;
    /*
     * 0 Is Wi-Fi info/no Wi-Fi info page
     * 1 Is fancy logs page
     * 2 Is all logs page
    */
    int currentPage = 0;
    QFile fancyLogs = QFile("/external_root/run/wifi_stats");
    QFile allLogs = QFile("/external_root/var/log/wifi.log");
    bool waitingForFile = false;
    QTimer getWifiInformationTimer;

private slots:
    void setWifiInfoPage();
    void setFancyLoggingPage();
    void setAllLogsPage();
    void on_nextBtn_clicked();
    void on_previousBtn_clicked();
    void changePage();
    void getWifiInformation();
    void on_returnBtn_clicked();
    void updateLogs();
    void on_refreshBtn_clicked();
};

#endif // WIFILOGGER_H
