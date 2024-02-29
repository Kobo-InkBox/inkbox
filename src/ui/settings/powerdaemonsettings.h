#ifndef POWERDAEMONSETTINGS_H
#define POWERDAEMONSETTINGS_H

#include <QWidget>

namespace Ui {
class PowerDaemonSettingsWidget;
}

class PowerDaemonSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit PowerDaemonSettingsWidget(QWidget *parent = nullptr);
    ~PowerDaemonSettingsWidget();
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

    void on_idleSleepIncreaseMBtn_clicked();

    void on_idleSleepDecreaseMBtn_clicked();

private:
    Ui::PowerDaemonSettingsWidget * ui;
    bool whenChargerSleepBool;
    bool chargerWakeUpBool;
    bool wifiReconnectBool;
    bool ledUsageBool;
    bool customCaseBool;
    bool deepSleepBool;
    int idleSleepInt;
    int cinematicBrightnessInt;
};
#endif // POWERDAEMONSETTINGS_H
