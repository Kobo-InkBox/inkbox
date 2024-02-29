#include "koboxsettings.h"
#include "device.h"
#include "functions.h"
#include "ui.h"
#include "ui_koboxsettings.h"

#include <QFile>
#include <QScreen>

KoboxSettingsWidget::KoboxSettingsWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::KoboxSettingsWidget) {
	ui->setupUi(this);
	KoboxSettingsWidget::setFont(QFont("u001"));
	ui->okBtn->setFont(QFont("Inter"));

	// UI tweaks
	ui->okBtn->setProperty("type", "borderless");
	ui->pushButton->setProperty("type", "borderless");
	ui->resetKoboxBtn->setProperty("type", "borderless");
	ui->pushButton->setStyleSheet("font-weight: bold");
	ui->resetKoboxBtn->setStyleSheet("font-weight: bold");
	ui->okBtn->setStyleSheet("font-weight: bold");

	// Stylesheet
	UI::applyStyle(*this, "eink");

	if (checkconfig("/external_root/boot/flags/X11_START") == true) {
		ui->koboxStatusLabel->setText("KoBox is <b>enabled</b>");
		not_user_change = true;
		ui->checkBox->click();
		ui->runtimeSettingsWidget->setVisible(true);
	} else {
		ui->koboxStatusLabel->setText("KoBox is <b>disabled</b>");
		ui->runtimeSettingsWidget->setVisible(false);
	}

	// DPI setting
	QString dpiSetting;
	QString dpiSettingStr = readFile(".config/00-kobox/dpiSetting");
	int dpi = dpiSettingStr.toInt();
	if (dpiSettingStr.isEmpty()) {
		dpi = Device::getSingleton()->getSingleton()->getBaseDPI();
		writeFile(".config/00-kobox/dpiSetting", QString(dpi));
	}

	ui->spinBox->setValue(dpi);
}

KoboxSettingsWidget::~KoboxSettingsWidget() {
	delete ui;
}

void KoboxSettingsWidget::on_okBtn_clicked() {
	KoboxSettingsWidget::close();
}

void KoboxSettingsWidget::on_checkBox_toggled(bool checked) {
	if (checked == true) {
		if (not_user_change != true) {
			log("Enabling KoBox subsystem", className);
			writeFile("/external_root/boot/flags/X11_START", "true\n");
			openSettingsRebootDialog();
		} else {
			not_user_change = false;
		}
	} else {
		log("Disabling KoBox subsystem", className);
		writeFile("/external_root/boot/flags/X11_START", "false\n");
		openSettingsRebootDialog();
	}
}

void KoboxSettingsWidget::openSettingsRebootDialog() {
	log("Showing reboot dialog", className);
	global::settings::settingsRebootDialog = true;
	global::kobox::koboxSettingsRebootDialog = true;
	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
}
void KoboxSettingsWidget::openResetKoboxDialog() {
	log("Showing KoBox reset dialog", className);
	global::kobox::resetKoboxDialog = true;
	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void KoboxSettingsWidget::on_spinBox_valueChanged(int arg1) {
	QString number = QString::number(arg1);
	writeFile(".config/00-kobox/dpiSetting", number);
	log("X11 DPI set to " + number, className);
}

void KoboxSettingsWidget::on_pushButton_clicked() {
	// Export Extensions disk image over USB with g_mass_storage/g_file_storage
	global::usbms::usbmsDialog = false;
	global::usbms::launchUsbms = true;
	global::usbms::koboxExportExtensions = true;

	log("Exporting KoBox extensions onboard storage via USB", className);
	log("Showing USBMS splash", className);
	usbmsWindow = new USBMSSplashWidget();
	usbmsWindow->setAttribute(Qt::WA_DeleteOnClose);
	usbmsWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
	usbmsWindow->show();
}

void KoboxSettingsWidget::on_resetKoboxBtn_clicked() {
	openResetKoboxDialog();
}
