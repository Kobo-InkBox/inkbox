#ifndef POWERDAEMONSETTINGS_H
#define POWERDAEMONSETTINGS_H

#include <QWidget>

namespace Ui {
class powerDaemonSettings;
}

class powerDaemonSettings : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit powerDaemonSettings(QWidget *parent = nullptr);
    ~powerDaemonSettings();
    int cinematicBrightnessMs;

private slots:
    void on_CBSIncreaseBtn_clicked();
    void on_CBSDecreaseBtn_clicked();
    void on_exitBtn_clicked();
    void on_expBtn_clicked();
    void on_hWhenChargerSleepBtn_clicked(bool checked);
    void on_hChargerWakeUpBtn_clicked(bool checked);
    void on_wifiReconnectBtn_clicked(bool checked);
    void on_ledUsageBtn_clicked(bool checked);
    void on_idleSleepIncreaseBtn_clicked();
    void on_idleSleepDecreaseBtn_clicked();
    void on_hCustomCaseBtn_clicked(bool checked);
    void on_deepSleepBtn_clicked(bool checked);

    void convertIdleSleepInt();
    void convertCinematicInt();

private:
    Ui::powerDaemonSettings * ui;
    bool whenChargerSleepBool;
    bool chargerWakeUpBool;
    bool wifiReconnectBool;
    bool ledUsageBool;
    int idleSleepInt;
    bool customCaseBool;
    bool deepSleepBool;
    int cinematicBrightnessInt;
};

#endif // POWERDAEMONSETTINGS_H
