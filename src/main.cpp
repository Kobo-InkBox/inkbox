/*
	InkBox: Open-source Qt-based eBook reader
	Copyright (C) 2021-2024 Nicolas Mailloux <nicolecrivain@gmail.com>
	SPDX-License-Identifier: GPL-3.0-only

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "audiothread.h"
#include "encryptionmanager.h"
#include "functions.h"
#include "generaldialog.h"
#include "mainwindow.h"
#include "reader.h"
#include "sleepdialog.h"
#include "sleepthread.h"
#include "splashAlert.h"
#include "ui.h"

#include "device.h"
#include <QApplication>
#include <QFile>
#include <QRect>
#include <QScreen>
#include <QTextCodec>
#include <QTextStream>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));

	if (char *debug = std::getenv("DEBUG")) {
		if (std::atoi(debug) == 1) {
			global::logger::status = true;
		}
	}

	QString deviceID = readFile("/opt/inkbox_device");
	if (deviceID.isEmpty()) {
		// we don't have a device id, assume that this is running in an emu
		deviceID = "emu";
	}
	log("Running on device " + deviceID, "main", true);
	// initialize the platform singleton
	Device::setup(deviceID);

	// Tell the OS that we're currently running
	writeFile("/tmp/inkbox_running", "true");

	setDefaultWorkDir();
	if (Device::getSingleton()->supportsWifi()) {
		log("Device has Wi-Fi capabilities", "main");
	} else {
		log("Device does not have Wi-Fi capabilities", "main");
	}

	if (QFile::exists("/tmp/currentlyRunningUserApplication")) {
		QFile::remove("/tmp/currentlyRunningUserApplication");
	}

	if (QFile::exists("/tmp/rescan_userapps")) {
		QFile::remove("/tmp/rescan_userapps");
		log("Re-scanning user applications from explicit request", "main");
		writeFile("/opt/ibxd", "gui_apps_stop\n");
		while (true) {
			if (QFile::exists("/tmp/gui_apps_stopped")) {
				QFile::remove("/tmp/gui_apps_stopped");
				break;
			}
			QThread::msleep(500);
		}
		writeFile("/opt/ibxd", "gui_apps_start\n");
		while (true) {
			if (QFile::exists("/tmp/gui_apps_started")) {
				if (checkconfig("/tmp/gui_apps_started") == true) {
					log("GUI apps service started successfully", "main");
					QFile::remove("/tmp/gui_apps_started");
					updateUserAppsMainJsonFile();
					break;
				} else {
					log("GUI apps service failed to start", "main");
					QFile::remove("/tmp/gui_apps_started");
					break;
				}
			}
			QThread::msleep(500);
		}
		updateUserAppsMainJsonFile();
	}

	// Power daemon-related stuff
	SleepDialog *sleepDialogWindow = new SleepDialog;
	// https://wiki.qt.io/QThreads_general_usage
	QThread *IPDThread = new QThread();
	SleepThread *sleepThreadWindow = new SleepThread();
	sleepThreadWindow->moveToThread(IPDThread);
	QObject::connect(IPDThread, &QThread::started, sleepThreadWindow, &SleepThread::start);
	QObject::connect(sleepThreadWindow, &SleepThread::startDialog, sleepDialogWindow, &SleepDialog::launchSleepDialog);
	QObject::connect(sleepThreadWindow, &SleepThread::stopDialog, sleepDialogWindow, &SleepDialog::hideSleepDialog);
	IPDThread->start();

	// Audio stuff
	if (checkconfig(".config/e-2-audio/enabled") == true) {
		log("Audio is enabled", "main");
		global::audio::enabled = true;
		QThread *audioThread = new QThread();
		AudioThread *audioObject = new AudioThread();
		audioObject->moveToThread(audioThread);
		QObject::connect(audioThread, &QThread::started, audioObject, &AudioThread::start);
		audioThread->start();
	} else {
		log("Audio is disabled", "main");
	}

	if (checkconfig(".config/18-encrypted_storage/status") == true and checkconfig("/external_root/run/encfs_mounted") == false) {
		// Open Encryption Manager to unlock encrypted storage
		CryptoManagerWidget w;
		const QScreen *screen = qApp->primaryScreen();
		w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
		w.show();
		return a.exec();
	} else if (checkconfig("/external_root/run/encfs_mounted") == true and checkconfig("/external_root/run/encfs_repack") == true) {
		log("Launching encryptionManager", "main");
		CryptoManagerWidget w;
		const QScreen *screen = qApp->primaryScreen();
		w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
		w.show();
		return a.exec();
	} else {
		// Variables
		global::reader::startBatteryWatchdog = false;
		global::reader::startUsbmsPrompt = false;

		// Checking if battery level is critical; if true (and if it is not charging), then display a "Please charge your eReader" splash and power off.
		if (Device::getSingleton()->getBatteryLevel() <= 5) {
			if (!Device::getSingleton()->isUSBPluggedIn()) {
				global::battery::showCriticalBatteryAlert = true;
				SplashAlertWidget w;

				const QScreen *screen = qApp->primaryScreen();
				w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
				w.show();
				return a.exec();
			}
		}

		// Checking if there has been an ALERT flag set up, and if there is, show a big warning
		if (checkconfig("/external_root/boot/flags/ALERT") == true) {
			SplashAlertWidget w;

			const QScreen *screen = qApp->primaryScreen();
			w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
			w.show();
			return a.exec();
		}
		// If we're waking from sleep and we have the lockscreen enabled, we'll "resume" the book from scratch
		else if (checkconfig("/tmp/suspendBook") == true && checkconfig("/inkbox/bookIsEpub") == false) {
			// Start the low/critical battery alert timer from the Reader framework since MainWindow is not going to be shown
			global::runningInstanceIsReaderOnly = true;
			global::reader::startBatteryWatchdog = true;
			global::reader::startUsbmsPrompt = true;
			global::reader::skipOpenDialog = true;

			writeFile("/inkbox/skip_opendialog", "true");

			ReaderWidget w;

			const QScreen *screen = qApp->primaryScreen();
			w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
			w.show();
			return a.exec();

		} else {
			QProcess::execute("remount_tmpfs_launch.sh", QStringList());

			MainWindow w;

			QApplication::setStyle("windows");
			UI::applyStyle(w, "eink");

			const QScreen *screen = qApp->primaryScreen();
			w.setGeometry(QRect(QPoint(0, 0), screen->geometry().size()));
			w.show();
			return a.exec();
		}
	}
}
