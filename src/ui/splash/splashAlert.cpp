#include "splashAlert.h"
#include "device.h"
#include "functions.h"
#include "ui.h"
#include "ui_splashAlert.h"

#include <QFile>
#include <QPixmap>
#include <QProcess>
#include <QScreen>

SplashAlertWidget::SplashAlertWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::SplashAlertWidget) {
	ui->setupUi(this);
	ui->messageLabel->setFont(QFont("u001"));

	// Getting the screen's size
	float sW = QGuiApplication::screens()[0]->size().width();
	float sH = QGuiApplication::screens()[0]->size().height();
	// Defining what the "Alert" icon size will be
	float stdIconWidth = sW / 1.7;
	float stdIconHeight = sH / 1.7;

	// General stylesheet
	UI::applyStyle(*this, ":/resources/eink-dark.qss");

	// Default icon in case none of the conditions below are met
	QPixmap pixmap(":/resources/alert.png");
	QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	ui->alertIconLabel->setPixmap(scaledPixmap);

	// Checking if the update's signature is untrusted. The signature error will always take precedence over the downgrade one (c.f. update.sh script)
	if (checkconfig("/external_root/boot/flags/ALERT_SIGN") == true) {
		QPixmap pixmap(":/resources/alert.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->alertIconLabel->setPixmap(scaledPixmap);

		signatureError = true;
		log("Displaying signature error alert splash", className);
		ui->securityLabel->setText("Failed to update InkBox.");
		ui->messageLabel->setText("The digital signature of the update is untrusted.\nFor security reasons, it cannot be installed.");
		ui->stackedWidget->setCurrentIndex(1);
	}
	if (checkconfig("/external_root/boot/flags/ALERT_DOWNGRADE") == true) {
		QPixmap pixmap(":/resources/alert.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->alertIconLabel->setPixmap(scaledPixmap);

		log("Displaying downgrade error alert splash", className);
		downgradeError = true;
		ui->securityLabel->setText("Failed to update InkBox.");
		ui->messageLabel->setText("An error occured during the update process.\nThe update package's version is lower than the actual installed version.");
		ui->stackedWidget->setCurrentIndex(1);
	}
	if (global::encfs::lockdown == true) {
		ui->stackedWidget->setVisible(false);
		ui->stackedWidget->deleteLater();
		QPixmap pixmap(":/resources/alert.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->alertIconLabel->setPixmap(scaledPixmap);

		ui->warningLabel->setText("Fatal error");
		ui->securityLabel->setText("Device lockdown");
		log(global::encfs::lockdownMessage, className);
		QString message = "Due to multiple incorrect passphrase attempts, this device is locked down until\n" + global::encfs::unlockTime + "\nand won't boot.";
		ui->messageLabel->setText(message);
		poweroff(false);
		QTimer::singleShot(1000, this, SLOT(quit()));
	}
	if (global::battery::showCriticalBatteryAlert == true) {
		global::battery::showCriticalBatteryAlert = false;
		ui->stackedWidget->setVisible(false);
		ui->stackedWidget->deleteLater();

		QPixmap pixmap(":/resources/battery_alert.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->alertIconLabel->setPixmap(scaledPixmap);

		log("Displaying critical battery alert splash", className);
		criticalBattery = true;
		ui->warningLabel->setText("Please charge your eReader.");
		ui->securityLabel->setText("The battery's charge level is critical.");
		ui->messageLabel->setText("To prevent damage, your device has been turned off.\nPlease consider charging it.");
		// The Mini (N705) has a higher DPI and otherwise the text doesn't fit
		Device::ScreenSize screenSize = Device::getSingleton()->getScreenSize();
		if (screenSize == Device::ScreenSize::SMALL) {
			ui->warningLabel->setStyleSheet("QLabel { background-color : black; color : white; font-size: 14pt }");
		} else {
			ui->warningLabel->setStyleSheet("QLabel { background-color : black; color : white; font-size: 16pt }");
		}
		poweroff(false);
		QTimer::singleShot(1000, this, SLOT(quit()));
	}

	ui->messageLabel->setStyleSheet("QLabel { background-color : black; color : white; font-size: 10pt }");
	ui->securityLabel->setStyleSheet("QLabel { background-color : black; color : white; font-size: 11pt }");
	ui->continueBtn->setProperty("type", "borderless");
	ui->resetBtn->setProperty("type", "borderless");
	ui->continueBtn->setStyleSheet("padding: 20px");
	ui->resetBtn->setStyleSheet("padding: 20px");
	ui->continue2Btn->setProperty("type", "borderless");
	ui->continue2Btn->setStyleSheet("padding: 20px");
}

SplashAlertWidget::~SplashAlertWidget() {
	delete ui;
}

void SplashAlertWidget::on_continueBtn_clicked() {
	// We continue anyway and re-set the ALERT flag
	writeFile("/external_root/boot/flags/ALERT", "false\n");
	writeFile("/external_root/boot/flags/ALERT_SIGN", "false\n");
	updateReset();
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void SplashAlertWidget::on_resetBtn_clicked() {
	// We set the DO_FACTORY_RESET flag and we restart the device
	log("Factory reset requested; setting required flags", className);
	writeFile("/external_root/boot/flags/DO_FACTORY_RESET", "true\n");
	writeFile("/external_root/boot/flags/DIAGS_BOOT", "true\n");
	QString reboot_prog("/sbin/reboot");
	QStringList reboot_args;
	reboot_args << "no_splash";
	QProcess *reboot_proc = new QProcess();
	reboot_proc->start(reboot_prog, reboot_args);
	reboot_proc->waitForFinished();
	reboot_proc->deleteLater();
}

void SplashAlertWidget::on_continue2Btn_clicked() {
	// We continue anyway and re-set the ALERT flag
	writeFile("/external_root/boot/flags/ALERT", "false\n");
	updateReset();

	log("Restarting InkBox", className);
	if (signatureError == true) {
		writeFile("/external_root/boot/flags/ALERT_SIGN", "false\n");
		QProcess process;
		process.startDetached("inkbox", QStringList());
		qApp->quit();
	}
	if (downgradeError == true) {
		writeFile("/external_root/boot/flags/ALERT_DOWNGRADE", "false\n");
		QProcess process;
		process.startDetached("inkbox", QStringList());
		qApp->quit();
	}
}

void SplashAlertWidget::updateReset() {
	writeFile("/mnt/onboard/onboard/.inkbox/can_really_update", "false\n");
	writeFile("/mnt/onboard/onboard/.inkbox/can_update", "false\n");
}

void SplashAlertWidget::quit() {
	qApp->quit();
}
