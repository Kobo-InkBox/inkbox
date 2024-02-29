#include "koboxappsdialog.h"
#include "ui_koboxappsdialog.h"

#include "device.h"
#include "functions.h"

#include "ui.h"
#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QMessageBox>
#include <QScreen>
#include <QStringListModel>

KoboxAppsDialog::KoboxAppsDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::KoboxAppsDialog) {
	ui->setupUi(this);
	ui->definitionLabel->setFont(QFont("u001"));
	ui->appsList->setFont(QFont("u001"));

	// Preventing outside interaction
	this->setModal(true);

	// Stylesheet, style & misc.
	UI::applyStyle(*this, "eink");

	ui->definitionLabel->setText("Please select an application.\nClick on 'Launch' to start it.");

	ui->launchBtn->setProperty("type", "borderless");
	ui->cancelBtn->setProperty("type", "borderless");
	ui->launchBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
	ui->cancelBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
	ui->appsList->setStyleSheet("font-size: 9pt");
	ui->definitionLabel->setStyleSheet("font-size: 9.5pt");
	ui->headerLabel->setStyleSheet("font-weight: bold");

	this->adjustSize();

	// Centering dialog
	QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
	int x = (screenGeometry.width() - this->width()) / 2;
	int y = (screenGeometry.height() - this->height()) / 2;
	this->move(x, y);

	checkApps();
	QStringListModel *model = new QStringListModel(this);
	QStringList list = apps.split("\n", QString::SkipEmptyParts);

	// Apps that aren't extensions
	list.prepend("Geany");

	if (checkconfig("/external_root/opt/root/rooted") == true) {
		list.prepend("KTerm");
	}

	model->setStringList(list);
	ui->appsList->setModel(model);
	ui->appsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void KoboxAppsDialog::checkApps() {
	QFile apps_list("/external_root/opt/X11/extensions_list");
	apps_list.open(QIODevice::ReadWrite);
	QTextStream in(&apps_list);
	apps = in.readAll();
	apps_list.close();
}

KoboxAppsDialog::~KoboxAppsDialog() {
	delete ui;
}

void KoboxAppsDialog::on_cancelBtn_clicked() {
	KoboxAppsDialog::close();
}

void KoboxAppsDialog::on_launchBtn_clicked() {
	index = ui->appsList->currentIndex();
	itemText = index.data(Qt::DisplayRole).toString();
	if (itemText.isEmpty()) {
		emit showToast("Please select an application");
	} else {
		// DPI setting
		QString initialDpiSetting = readFile(".config/00-kobox/dpiSetting");
		int dpi;
		if (initialDpiSetting.isEmpty()) {
			dpi = Device::getSingleton()->getBaseDPI();
		} else {
			dpi = initialDpiSetting.toInt();
		}

		// Fullscreen or windowed (i3)
		// Mostly windowed except for apps like KTerm which ship their own OSK
		dpModeSetting = "windowed";

		if (itemText == "Netsurf") {
			log("Launching KoBox app: NetSurf", className);
			// Bypass standard shell script launch shenanigans
			writeFile("/external_root/tmp/X_program", "!netsurf");
		} else if (itemText == "KTerm") {
			log("Launching KoBox app: KTerm", className);
			writeFile("/external_root/tmp/X_program", "/usr/local/bin/kterm -l /usr/local/share/kterm/layouts/keyboard-kt.xml -k 1");
			dpModeSetting = "fullscreen";
			dpi += 50;
		} else if (itemText == "Geany") {
			log("Launching KoBox app: Geany", className);
			writeFile("/external_root/tmp/X_program", "geany");
		} else {
			log("Launching KoBox app: " + itemText, className);
			QString app = itemText.toLower();
			writeFile("/external_root/tmp/X_program", app);
		}

		writeFile("/external_root/tmp/X_dpmode", dpModeSetting);
		writeFile("/external_root/tmp/X_dpi", QString(dpi));

		/* Wheeee! */
		global::kobox::showKoboxSplash = true;

		// Re-use USBMS splash window for KoBox splash, since it's pretty much the same layout
		log("Showing KoBox splash", className);
		usbmsSplashWindow = new USBMSSplashWidget();
		usbmsSplashWindow->setAttribute(Qt::WA_DeleteOnClose);
		usbmsSplashWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
		usbmsSplashWindow->show();
		QApplication::processEvents();

		// Stop EncFS/Encrypted storage
		if (checkconfig("/external_root/run/encfs_mounted") == true) {
			writeFile("/external_root/run/encfs_stop_cleanup", "true");
			writeFile("/opt/ibxd", "encfs_stop\n");
		}

		// Write to FIFO to start X11
		writeFile("/opt/ibxd", "x_start_gui\n");
	}
}
