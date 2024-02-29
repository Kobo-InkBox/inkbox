#include "usbmsSplash.h"
#include "device.h"
#include "ui.h"
#include "ui_usbmsSplash.h"

#include <QPixmap>
#include <QScreen>
#include <QTimer>

#include "functions.h"

USBMSSplashWidget::USBMSSplashWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::USBMSSplashWidget) {
	ui->setupUi(this);
	USBMSSplashWidget::setFont(QFont("u001"));
	ui->label->setFont(QFont("Inter"));

	// Getting the screen's size
	sW = QGuiApplication::screens()[0]->size().width();
	sH = QGuiApplication::screens()[0]->size().height();

	// Defining what the default icon size will be
	if (global::kobox::showKoboxSplash == true) {
		float stdIconWidth = sW / 1.30;
		float stdIconHeight = sH / 1.30;

		// Stylesheet
		UI::applyStyle(*this, "eink");

		ui->label->setFont(QFont("Inter"));
		ui->label->setText("Launching KoBox subsystem");
		ui->label->setStyleSheet("font-size: 14pt; font-weight: bold");
		ui->label_3->setText("Please wait, this could take a while.");
		ui->label_3->setStyleSheet("font-size: 10pt");

		QPixmap pixmap(":/resources/kobox-icon.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->label_2->setPixmap(scaledPixmap);
	} else {
		float stdIconWidth = sW / 1.15;
		float stdIconHeight = sH / 1.15;

		this->setStyleSheet("background-color:black;");
		ui->label->setStyleSheet("QLabel { background-color : black; color : white; font-size: 15pt; font-weight: bold }");
		ui->label_3->setStyleSheet("QLabel { background-color : black; color : white; font-size: 10pt }");

		QPixmap pixmap(":/resources/usbms.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->label_2->setPixmap(scaledPixmap);
	}

	if (global::usbms::launchUsbms == true) {
		global::usbms::launchUsbms = false;
		usbmsLaunch();
	}
}

void USBMSSplashWidget::usbmsLaunch() {
	Device *device = Device::getSingleton();

	log("Entering USBMS session", className);
	writeFile("/tmp/in_usbms", "true");
	QTimer::singleShot(1500, this, SLOT(brightnessDown()));
	QTimer::singleShot(1500, this, SLOT(warmthDown()));

	if (global::usbms::koboxExportExtensions == true) {
		writeFile("/opt/ibxd", "kobox_extensions_storage_unmount\n");
	}
	if (checkconfig("/external_root/run/encfs_mounted") == true) {
		writeFile("/external_root/run/encfs_stop_cleanup", "true");
		writeFile("/opt/ibxd", "encfs_stop\n");
		while (true) {
			if (QFile::exists("/tmp/encfs_stopped")) {
				QFile::remove("/tmp/encfs_stopped");
				break;
			}
			QThread::msleep(500);
		}
	}

	writeFile("/opt/ibxd", "gui_apps_stop\n");
	while (true) {
		if (QFile::exists("/tmp/gui_apps_stopped")) {
			QFile::remove("/tmp/gui_apps_stopped");
			break;
		}
		QThread::msleep(500);
	}

	writeFile("/opt/ibxd", "onboard_unmount\n");
	while (true) {
		if (QFile::exists("/tmp/onboard_unmounted")) {
			QFile::remove("/tmp/onboard_unmounted");
			break;
		}
		QThread::msleep(500);
	}

	writeFile("/opt/ibxd", "usbnet_stop\n");
	while (true) {
		if (QFile::exists("/tmp/usbnet_stopped")) {
			QFile::remove("/tmp/usbnet_stopped");
			break;
		}
		QThread::msleep(500);
	}

	device->loadUSBMassStorageModule();

	QString prog_1("insmod");
	QStringList args_1;

	massStorageModule = device->getUSBMassStorageModule();

	if (global::usbms::koboxExportExtensions == true) {
		args_1 << massStorageModule << "file=/external_root/opt/storage/X11/extensions-user.img"
			   << "removable=y"
			   << "stall=0";
	} else {
		args_1 << massStorageModule << "file=/external_root/opt/storage/onboard"
			   << "removable=y"
			   << "stall=0";
	}

	QProcess *proc_1 = new QProcess();
	proc_1->start(prog_1, args_1);
	proc_1->waitForFinished();
	proc_1->deleteLater();

	bool exitUsbMsDone = false;
	QTimer *usbms_t = new QTimer(this);
	usbms_t->setInterval(1000);
	connect(usbms_t, &QTimer::timeout, [&]() {
		if (exitUsbMsDone == false) {
			if (!device->isUSBPluggedIn()) {
				if (global::usbms::koboxExportExtensions == true) {
					reboot(false);
					qApp->quit();
				} else {
					log("Exiting USBMS session", className);
					// '<font/>' bit: because nothing else works ...
					ui->label->setText("<font face='Inter'>Processing content</font>");
					ui->label->setFont(QFont("Inter"));
					ui->label_3->setText("Please wait");
					ui->label_3->setFont(QFont("u001"));
					ui->label->setStyleSheet("QLabel { background-color : black; color : white; font-size: 15pt; font-weight: bold }");
					ui->label_3->setStyleSheet("QLabel { background-color : black; color : white; font-size: 11pt }");
					ui->label->setFont(QFont("u001"));

					float stdIconWidth = sW / 2;
					float stdIconHeight = sH / 2;

					QPixmap pixmap(":/resources/clock-inverted.png");
					QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
					ui->label_2->setPixmap(scaledPixmap);

					this->repaint();

					QString prog("rmmod");
					QStringList args;
					args << massStorageModule;
					QProcess *proc = new QProcess();
					proc->start(prog, args);
					proc->waitForFinished();
					proc->deleteLater();

					QTimer::singleShot(500, this, SLOT(restartServices()));
				}
				exitUsbMsDone = true;
			} else {
				;
			}
		}
	});
	usbms_t->start();
}

USBMSSplashWidget::~USBMSSplashWidget() {
	delete ui;
}

void USBMSSplashWidget::brightnessDown() {
	UI::transitionBrightness(0);
	writeFile("/tmp/inkbox-cinematicBrightness_ran", "false");
}

void USBMSSplashWidget::warmthDown() {
	if (Device::getSingleton()->supportsWarmLight()) {
        UI::transitionWarmth(0);
	}
}

void USBMSSplashWidget::quit_restart() {
	// If existing, cleaning bookconfig_mount mountpoint
	writeFile("/opt/ibxd", "bookconfig_unmount\n");

	// Restarting InkBox
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void USBMSSplashWidget::restartServices() {
	// Restarting USBNet
	// NOTE: USBNet is only started if required conditions are met (see https://github.com/Kobo-InkBox/rootfs/blob/master/etc/init.d/usbnet)
	writeFile("/opt/ibxd", "usbnet_start\n");
	while (true) {
		if (QFile::exists("/tmp/usbnet_started")) {
			QFile::remove("/tmp/usbnet_started");
			break;
		}
		QThread::msleep(500);
	}

	// Mounting onboard storage
	writeFile("/opt/ibxd", "onboard_mount\n");
	while (true) {
		if (QFile::exists("/tmp/onboard_mounted")) {
			QFile::remove("/tmp/onboard_mounted");
			break;
		}
		QThread::msleep(500);
	}

	// Checking for updates
	writeFile("/opt/ibxd", "update_inkbox_restart\n");
	while (true) {
		if (QFile::exists("/tmp/update_inkbox_restarted")) {
			QFile::remove("/tmp/update_inkbox_restarted");
			break;
		}
		QThread::msleep(500);
	}

	QFile::remove("/tmp/in_usbms");
	// GUI apps: update main JSON file
	writeFile("/opt/ibxd", "gui_apps_start\n");
	while (true) {
		if (QFile::exists("/tmp/gui_apps_started")) {
			if (checkconfig("/tmp/gui_apps_started") == true) {
				QFile::remove("/tmp/gui_apps_started");
				updateUserAppsMainJsonFile();
				break;
			} else {
				log("GUI apps service failed to start", className);
				QFile::remove("/tmp/gui_apps_started");
				break;
			}
		}
		QThread::msleep(500);
	}
	// Remove macOS dotfiles
	{
		QString prog("busybox-initrd");
		QStringList args;
		args << "find"
			 << "/mnt/onboard/onboard"
			 << "-type"
			 << "f"
			 << "-name"
			 << "._*"
			 << "-delete";
		QProcess *proc = new QProcess();
		proc->start(prog, args);
		proc->waitForFinished();
		proc->deleteLater();
	}
	// Re-generate local library on next launch
	QFile::remove(global::localLibrary::databasePath);

	quit_restart();
}
