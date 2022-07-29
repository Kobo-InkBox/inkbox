#include "powerdaemonsettings.h"
#include "ui_powerdaemonsettings.h"
#include "functions.h"

#include <QFile>

powerDaemonSettings::powerDaemonSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::powerDaemonSettings)
{
    ui->setupUi(this);
    powerDaemonSettings::setFont(QFont("u001"));
    ui->mainLabel->setFont(QFont("Inter"));

    // Avoid some display issues (label moving because width of text would change with a proportional font)
    ui->CBSLabel->setFont(QFont("Noto Sans Mono"));
    ui->idleSleepLabel->setFont(QFont("Noto Sans Mono"));

    ui->label_2->setFont(QFont("Inter"));
    ui->exitBtn->setFont(QFont("Inter"));

    // UI tweaks
    ui->exitBtn->setProperty("type", "borderless");
    ui->expBtn->setProperty("type", "borderless");
    ui->hWhenChargerSleepBtn->setProperty("type", "borderless");
    ui->hChargerWakeUpBtn->setProperty("type", "borderless");
    ui->wifiReconnectBtn->setProperty("type", "borderless");
    ui->ledUsageBtn->setProperty("type", "borderless");
    ui->hCustomCaseBtn->setProperty("type", "borderless");
    ui->deepSleepBtn->setProperty("type", "borderless");

    ui->mainLabel->setStyleSheet("font-weight: bold");
    ui->CBSLabel->setStyleSheet("font-weight: bold");
    ui->idleSleepLabel->setStyleSheet("font-weight: bold");
    ui->exitBtn->setStyleSheet("font-weight: bold");
    // Experimental features label
    ui->label_2->setStyleSheet("font-weight: bold");

    /*
    if(global::deviceID == "n306")
    {
        ui->wifiReconnectBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->ledUsageBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->deepSleepBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->hWhenChargerSleepBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->hChargerWakeUpBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->hCustomCaseBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->wifiReconnectBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
        ui->wifiReconnectBtn->setStyleSheet("QCheckBox::indicator { width:50px; height: 50px; }");
    }
    */


    // Icons
    ui->CBSDecreaseBtn->setIcon(QIcon(":/resources/minus.png"));
    ui->CBSDecreaseBtn->setFixedWidth(80);
    ui->CBSIncreaseBtn->setIcon(QIcon(":/resources/plus.png"));
    ui->CBSIncreaseBtn->setFixedWidth(80);

    ui->idleSleepDecreaseBtn->setIcon(QIcon(":/resources/minus.png"));
    ui->idleSleepDecreaseBtn->setFixedWidth(80);
    ui->idleSleepIncreaseBtn->setIcon(QIcon(":/resources/plus.png"));
    ui->idleSleepIncreaseBtn->setFixedWidth(80);

    // Padding
    ui->CBSDecreaseBtn->setStyleSheet("padding: 10px");
    ui->CBSIncreaseBtn->setStyleSheet("padding: 10px");
    ui->idleSleepDecreaseBtn->setStyleSheet("padding: 10px");
    ui->idleSleepIncreaseBtn->setStyleSheet("padding: 10px");

    // Stylesheet
    QFile stylesheetFile("/mnt/onboard/.adds/inkbox/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    this->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();

    // Button tweaks
    ui->CBSDecreaseBtn->setAutoRepeat(true);
    ui->CBSIncreaseBtn->setAutoRepeat(true);
    ui->idleSleepDecreaseBtn->setAutoRepeat(true);
    ui->idleSleepIncreaseBtn->setAutoRepeat(true);

    // Hide items
    ui->hLabel_3->hide();
    ui->hCpuFreqComboBox->hide();

    ui->hWhenChargerSleepBtn->hide();
    ui->hChargerWakeUpBtn->hide();
    ui->hCustomCaseBtn->hide();

    // Prepare variables from system configuration (20-sleep_daemon)
    log("Reading variables from power daemon configuration", className);

    // 1 - cinematicBrightnessDelayMs
    QString cinematicBrightnessMs = readFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/1-cinematicBrightnessDelayMs");

    cinematicBrightnessInt = cinematicBrightnessMs.toInt();
    convertCinematicInt();

    // 2- cpuGovernor
    QString cpuGovernor = readFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/2-cpuGovernor");

    QStringList cpuGovernorList = QStringList{cpuGovernor};
    if(cpuGovernorList.contains("interactive") == false) {
        cpuGovernorList.append("interactive");
    }
    if(cpuGovernorList.contains("conservative") == false) {
        cpuGovernorList.append("conservative");
    }
    if(cpuGovernorList.contains("userspace") == false) {
        cpuGovernorList.append("userspace");
    }
    if(cpuGovernorList.contains("powersave") == false) {
        cpuGovernorList.append("powersave");
    }
    if(cpuGovernorList.contains("ondemand") == false) {
        cpuGovernorList.append("ondemand");
    }
    if(cpuGovernorList.contains("performance") == false) {
        cpuGovernorList.append("performance");
    }

    ui->hCpuFreqComboBox->addItems(cpuGovernorList);
    ui->hCpuFreqComboBox->setCurrentIndex(0);

    // 3 - whenChargerSleep
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/3-whenChargerSleep") == true) {
        whenChargerSleepBool = true;
	    ui->hWhenChargerSleepBtn->click();
    }
    else {
        whenChargerSleepBool = false;
    }

    // 4 - chargerWakeUp
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/4-chargerWakeUp") == true) {
        chargerWakeUpBool = true;
	    ui->hChargerWakeUpBtn->click();
    }
    else {
        chargerWakeUpBool = false;
    }

    // 5 - wifiReconnect
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/5-wifiReconnect") == true) {
        ui->wifiReconnectBtn->click();
        wifiReconnectBool = true;
    }
    else {
        wifiReconnectBool = false;
    }

    // 6 - ledUsage
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/6-ledUsage") == true) {
        ui->ledUsageBtn->click();
        ledUsageBool = true;
    }
    else {
        ledUsageBool = false;
    }

    // 7 - idleSleep
    QString idleSleepString = readFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/7-idleSleep");
    idleSleepInt = idleSleepString.replace("\n", "").toInt();
    convertIdleSleepInt();

    // 8 - customCase
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/8-customCase") == true) {
        ui->hCustomCaseBtn->click();
        customCaseBool = true;
    }
    else {
        customCaseBool = false;
    }

    // 9 - deepSleep
    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/9-deepSleep") == true) {
        ui->deepSleepBtn->click();
        deepSleepBool = true;
    }
    else {
        deepSleepBool = false;
    }
}

powerDaemonSettings::~powerDaemonSettings()
{
    delete ui;
}

void powerDaemonSettings::on_CBSIncreaseBtn_clicked()
{
    if(cinematicBrightnessInt < 500)
    {
        cinematicBrightnessInt = cinematicBrightnessInt + 1;
        convertCinematicInt();
    }
}

void powerDaemonSettings::on_CBSDecreaseBtn_clicked()
{
    if(cinematicBrightnessInt != 0)
    {
        cinematicBrightnessInt = cinematicBrightnessInt - 1;
        convertCinematicInt();
    }
}

void powerDaemonSettings::on_exitBtn_clicked()
{
    // Save all to files
    // 1 - cinematicBrightnessdelayMs
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/1-cinematicBrightnessDelayMs", QString::number(cinematicBrightnessInt));
    // 2 - cpuGovernor
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/2-cpuGovernor", ui->hCpuFreqComboBox->currentText());
    // 3 - whenChargerSleep
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/3-whenChargerSleep", QVariant(whenChargerSleepBool).toString());
    // 4 - chargerWakeUp
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/4-chargerWakeUp", QVariant(chargerWakeUpBool).toString());
    // 5 - wifiReconnect
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/5-wifiReconnect", QVariant(wifiReconnectBool).toString());
    // 6 - ledUsage
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/6-ledUsage", QVariant(ledUsageBool).toString());
    // 7 - idleSleep
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/7-idleSleep", QString::number(idleSleepInt));
    // 8 - customCase
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/8-customCase", QVariant(customCaseBool).toString());
    // 9 - deepSleep
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/9-deepSleep", QVariant(deepSleepBool).toString());

    // Notify power daemon of configuration update
    writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/updateConfig", "true");

    powerDaemonSettings::close();
}

void powerDaemonSettings::on_expBtn_clicked()
{
    // TODO: Implement additional warning
    ui->hLabel_3->show();
    ui->hCpuFreqComboBox->show();

    ui->hWhenChargerSleepBtn->show();
    ui->hChargerWakeUpBtn->show();
    ui->hCustomCaseBtn->show();

    ui->expBtn->hide();
}

void powerDaemonSettings::on_hWhenChargerSleepBtn_clicked(bool checked)
{
    QString settingString = "suspend when charging";
    if(checked == true) {
        logEnabled(settingString, className);
        whenChargerSleepBool = true;
    }
    else {
        logDisabled(settingString, className);
        whenChargerSleepBool = false;
    }
}

void powerDaemonSettings::on_hChargerWakeUpBtn_clicked(bool checked)
{
    QString settingString = "ignore charger wake-up events";
    if(checked == true) {
        logEnabled(settingString, className);
        chargerWakeUpBool = true;
    }
    else {
        logDisabled(settingString, className);
        chargerWakeUpBool = false;
    }
}

void powerDaemonSettings::on_wifiReconnectBtn_clicked(bool checked)
{
    QString settingString = "Wi-Fi reconnection on wake-up";
    if(checked == true) {
        logEnabled(settingString, className);
        wifiReconnectBool = true;
    }
    else {
        logDisabled(settingString, className);
        wifiReconnectBool = false;
    }
}

void powerDaemonSettings::on_ledUsageBtn_clicked(bool checked)
{
    QString settingString = "use LED";
    if(checked == true) {
        logEnabled(settingString, className);
        ledUsageBool = true;
    }
    else {
        logDisabled(settingString, className);
        ledUsageBool = false;
    }
}

void powerDaemonSettings::on_idleSleepIncreaseBtn_clicked()
{
    if(idleSleepInt >= 15) {
        idleSleepInt = idleSleepInt + 1;
    }
    else {
        idleSleepInt = 15;
    }
    convertIdleSleepInt();
}

void powerDaemonSettings::on_idleSleepDecreaseBtn_clicked() {
    if(idleSleepInt >= 15) {
        idleSleepInt = idleSleepInt - 1;
        if(idleSleepInt == 14) {
            idleSleepInt = 0;
        }
    }
    else {
        idleSleepInt = 0;
    }

    convertIdleSleepInt();
}

void powerDaemonSettings::convertIdleSleepInt()
{
    int copiedIdleSleepInt = idleSleepInt;
    int minutes = 0;
    int seconds = 0;
    if(copiedIdleSleepInt == 0) {
        ui->idleSleepLabel->setText("Never");
        return;
    }
    while(copiedIdleSleepInt > 60) {
        minutes = minutes + 1;
        copiedIdleSleepInt = copiedIdleSleepInt - 60;
    }
    seconds = copiedIdleSleepInt;

    QString text;
    if(minutes != 0) {
        text.append(QString::number(minutes) + "m ");
    }
    if(seconds != 0) {
        text.append(QString::number(seconds) + "s");
    }
    ui->idleSleepLabel->setText(text);
}

void powerDaemonSettings::on_hCustomCaseBtn_clicked(bool checked)
{
    QString settingString = "custom case";
    if(checked == true) {
        logEnabled(settingString, className);
        customCaseBool = true;
    }
    else {
        logDisabled(settingString, className);
        customCaseBool = false;
    }
}

void powerDaemonSettings::on_deepSleepBtn_clicked(bool checked)
{
    QString settingString = "deep sleep";
    if(checked == true) {
        logEnabled(settingString, className);
        deepSleepBool = true;
    }
    else {
        logDisabled(settingString, className);
        deepSleepBool = false;
    }
}

void powerDaemonSettings::convertCinematicInt()
{
    QString text = QString::number(cinematicBrightnessInt);
    // To avoid moving other widgets when the value changes
    text.append("ms");
    ui->CBSLabel->setText(text);


}
