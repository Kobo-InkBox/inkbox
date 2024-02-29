#include "settings.h"
#include "device.h"
#include "functions.h"
#include "mainwindow.h"
#include "ui.h"
#include "ui_mainwindow.h"
#include "ui_settings.h"

#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QProcess>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QWidget>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

using namespace std;

SettingsWidget::SettingsWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::SettingsWidget) {
	Device *device = Device::getSingleton();

	ui->setupUi(this);
	SettingsWidget::setFont(QFont("u001"));
	ui->comboBox->setFont(QFont("u001"));
	ui->tzComboBox->setFont(QFont("u001"));
	ui->aboutBtn->setFont(QFont("u001"));
	ui->headerBtn->setFont(QFont("Inter"));
	ui->headerLabel->setFont(QFont("Chivo"));
	ui->readingSettingsBtn->setFont(QFont("Chivo"));
	ui->homeSettingsBtn->setFont(QFont("Chivo"));
	ui->librarySettingsBtn->setFont(QFont("Chivo"));
	ui->storageSettingsBtn->setFont(QFont("Chivo"));
	ui->systemSettingsBtn->setFont(QFont("Chivo"));
	ui->securitySettingsBtn->setFont(QFont("Chivo"));
	ui->okBtn->setFont(QFont("Inter"));

	ui->setPasscodeBtn->setProperty("type", "borderless");
	ui->okBtn->setProperty("type", "borderless");
	ui->aboutBtn->setProperty("type", "borderless");
	ui->requestLeaseBtn->setProperty("type", "borderless");
	ui->usbmsBtn->setProperty("type", "borderless");
	ui->updateBtn->setProperty("type", "borderless");
	ui->resetBtn->setProperty("type", "borderless");
	ui->showSystemInfoBtn->setProperty("type", "borderless");
	ui->generateSystemReportBtn->setProperty("type", "borderless");
	ui->checkOtaUpdateBtn->setProperty("type", "borderless");
	ui->repackBtn->setProperty("type", "borderless");
	ui->exportHighlightsBtn->setProperty("type", "borderless");
	ui->label->setStyleSheet("font-size: 10.5pt; font-weight: bold");
	ui->okBtn->setStyleSheet("font-weight: bold");
	ui->aboutBtn->setStyleSheet("font-size: 9pt");
	ui->requestLeaseBtn->setStyleSheet("font-size: 9pt");
	ui->usbmsBtn->setStyleSheet("font-size: 9pt");
	ui->updateBtn->setStyleSheet("font-size: 9pt");
	ui->resetBtn->setStyleSheet("font-size: 9pt");
	ui->showSystemInfoBtn->setStyleSheet("font-size: 9pt");
	ui->generateSystemReportBtn->setStyleSheet("font-size: 9pt");
	ui->checkOtaUpdateBtn->setStyleSheet("font-size: 9pt");
	ui->comboBox->setStyleSheet("font-size: 9pt");
	ui->tzComboBox->setStyleSheet("font-size: 9pt");
	ui->setPasscodeBtn->setStyleSheet("font-size: 9pt");
	ui->repackBtn->setStyleSheet("font-size: 9pt");
	ui->exportHighlightsBtn->setStyleSheet("font-size: 9pt");

	// Getting the screen's size
	sW = QGuiApplication::screens()[0]->size().width();
	sH = QGuiApplication::screens()[0]->size().height();

	// Defining what the default icon size will be
	if (device->getScreenSize() == Device::ScreenSize::SMALL) {
		homeIconWidth = sW / 18;
		homeIconHeight = sW / 18;
	} else {
		homeIconWidth = sW / 20;
		homeIconHeight = sW / 20;
	}

	ui->headerStackedWidget->hide();

	ui->headerBtn->setIcon(QIcon(":/resources/chevron-left.png"));
	ui->headerBtn->setProperty("type", "borderless");
	ui->headerBtn->setStyleSheet("font-size: 9pt; font-weight: bold; padding: 15px");
	ui->headerLabel->setStyleSheet("padding: 15px");

	ui->readingSettingsBtn->setText("\t\t\tReading");
	ui->readingSettingsBtn->setProperty("type", "borderless");
	ui->readingSettingsBtn->setStyleSheet("padding: 25px; Text-align: left");
	ui->readingSettingsBtn->setIcon(QIcon(":/resources/book.png"));
	ui->readingSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->homeSettingsBtn->setText("\t\t\tHome");
	ui->homeSettingsBtn->setProperty("type", "borderless");
	ui->homeSettingsBtn->setStyleSheet("padding: 25px; Text-align:left");
	ui->homeSettingsBtn->setIcon(QIcon(":/resources/home.png"));
	ui->homeSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->librarySettingsBtn->setText("\t\t\t Library");
	ui->librarySettingsBtn->setProperty("type", "borderless");
	ui->librarySettingsBtn->setStyleSheet("padding: 25px; Text-align:left");
	ui->librarySettingsBtn->setIcon(QIcon(":/resources/online-library.png"));
	ui->storageSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->storageSettingsBtn->setText("\t\t\tStorage");
	ui->storageSettingsBtn->setProperty("type", "borderless");
	ui->storageSettingsBtn->setStyleSheet("padding: 25px; Text-align:left");
	ui->storageSettingsBtn->setIcon(QIcon(":/resources/usbms-inverted.png"));
	ui->storageSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->systemSettingsBtn->setText("\t\t\tSystem");
	ui->systemSettingsBtn->setProperty("type", "borderless");
	ui->systemSettingsBtn->setStyleSheet("padding: 25px; Text-align:left");
	ui->systemSettingsBtn->setIcon(QIcon(":/resources/settings.png"));
	ui->systemSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->securitySettingsBtn->setText("\t\t\tSecurity");
	ui->securitySettingsBtn->setProperty("type", "borderless");
	ui->securitySettingsBtn->setStyleSheet("padding: 25px; Text-align:left");
	ui->securitySettingsBtn->setIcon(QIcon(":/resources/lock.png"));
	ui->securitySettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));

	ui->usbNetworkingCheckBox->hide();
	ui->requestLeaseBtn->hide();
	ui->usbmsBtn->hide();
	ui->label_4->hide();
	ui->label_6->hide();
	ui->updateBtn->hide();
	ui->updateLabel->hide();
	ui->enableLockscreenCheckBox->hide();
	ui->setPasscodeBtn->hide();
	ui->setPasscodeLabel->hide();

	// Variables
	defineDefaultPageSize(0);
	defineDefaultPageSize(1);

	// Settings tweaking + enabling specific features whether it's running on the provided integrated OS or Kobo firmware
	if (checkconfig(".config/01-demo/config") == true) {
		ui->demoCheckBox->click();
	}
	if (checkconfig(".config/02-clock/config") == true) {
		ui->clockCheckBox->click();
	}
	if (checkconfig(".config/05-quote/config") == true) {
		ui->quoteCheckBox->click();
	}

	if (device->supportsNightmode()) {
		if (checkconfig(".config/10-dark_mode/config") == true) {
			ui->darkModeCheckBox->click();
		}
	} else {
		ui->darkModeCheckBox->hide();
		ui->darkModeCheckBox->deleteLater();
	}

	// Words number
	QString wordsNumberConfigStr = readFile(".config/07-words_number/config");
	if (wordsNumberConfigStr.isEmpty()) {
		Device::ScreenSize size = device->getScreenSize();
		if (size == Device::ScreenSize::SMALL) {
			wordsNumberSaved = 120;
		} else if (size == Device::ScreenSize::VERY_LARGE) {
			wordsNumberSaved = 250;
		} else {
			wordsNumberSaved = 175;
		}
	} else {
		QString words_number = wordsNumberConfigStr;
		wordsNumberSaved = wordsNumberConfigStr.toInt();
	}
	ui->wordsNumberValueLabel->setText(QString::number(wordsNumberSaved));

	// ePUB page size
	if (checkconfig(".config/13-epub_page_size/set") == true) {
		QString epubPageWidthSettingStr = readFile(".config/13-epub_page_size/width");
		if (!epubPageWidthSettingStr.isEmpty()) {
			QString pageWidth = epubPageWidthSettingStr;
			ui->pageSizeWidthLabel->setText(pageWidth);
			pageSizeWidthSaved = pageWidth.toInt();
		} else {
			// Failsafe: setting default
			ui->pageSizeWidthLabel->setText(QString::number(defaultEpubPageWidth));
			pageSizeWidthSaved = defaultEpubPageWidth;
		}
		QString epubPageHeightSettingStr = readFile(".config/13-epub_page_size/height");
		if (!epubPageHeightSettingStr.isEmpty()) {
			QString pageHeight = epubPageHeightSettingStr;
			ui->pageSizeHeightLabel->setText(pageHeight);
			pageSizeHeightSaved = pageHeight.toInt();
		} else {
			// Failsafe: setting default
			ui->pageSizeHeightLabel->setText(QString::number(defaultEpubPageHeight));
			pageSizeHeightSaved = defaultEpubPageHeight;
		}
	} else {
		ui->pageSizeWidthLabel->setText(QString::number(defaultEpubPageWidth));
		ui->pageSizeHeightLabel->setText(QString::number(defaultEpubPageHeight));
		pageSizeHeightSaved = defaultEpubPageHeight;
		pageSizeWidthSaved = defaultEpubPageWidth;
	}

	// QTextEdit scroll bar in Reader framework
	if (checkconfig(".config/14-reader_scrollbar/config") == true) {
		ui->readerScrollBarCheckBox->click();
	}

	// Sticky menubar
	if (checkconfig(".config/11-menubar/sticky") == true) {
		ui->menuBarCheckBox->click();
	}

	// Refresh
	QString refreshSettingStr = readFile(".config/04-book/refresh");
	if (refreshSettingStr.isEmpty()) {
		// Set default option, 3
		writeFile(".config/04-book/refresh", "3");
		ui->comboBox->setCurrentText("3 pages");
	} else {
		int refreshInt = refreshSettingStr.toInt();
		if (refreshInt == -1) {
			ui->comboBox->setCurrentText("Never refresh");
		}
		if (refreshInt == 0) {
			ui->comboBox->setCurrentText("Every page");
		}
		if (refreshInt == 1) {
			ui->comboBox->setCurrentText("1 page");
		}
		if (refreshInt == 2) {
			ui->comboBox->setCurrentText("2 pages");
		}
		if (refreshInt == 3) {
			ui->comboBox->setCurrentText("3 pages");
		}
		if (refreshInt == 4) {
			ui->comboBox->setCurrentText("4 pages");
		}
		if (refreshInt == 5) {
			ui->comboBox->setCurrentText("5 pages");
		}
		if (refreshInt == 6) {
			ui->comboBox->setCurrentText("6 pages");
		}
	}

	// Lock screen
	if (checkconfig(".config/12-lockscreen/config") == true) {
		ui->enableLockscreenCheckBox->click();
	}

	// Global reading settings
	if (checkconfig(".config/16-global_reading_settings/config") == true) {
		ui->globalReadingSettingsCheckBox->click();
	}

	// Encrypted storage
	if (checkconfig(".config/18-encrypted_storage/status") == true) {
		ui->enableEncryptedStorageCheckBox->click();
	} else {
		// Next interaction will be by the user
		enableEncryptedStorageUserChange = true;
	}
	if (getEncFSStatus() == false) {
		ui->repackLabel->hide();
		ui->repackBtn->hide();
	}

	// Timezone
	ui->tzComboBox->addItems(QStringList(readFile(":/resources/tzlist").split("\n", Qt::SkipEmptyParts)));
	timezone_not_user_change = true;
	if (readFile(".config/19-timezone/config-name").isEmpty()) {
		ui->tzComboBox->setCurrentText("UTC");
	} else {
		ui->tzComboBox->setCurrentText(readFile(".config/19-timezone/config-name"));
	}

	// Local library folders support
	if (checkconfig(".config/21-local_library/folders") == true) {
		ui->localLibraryShowFoldersCheckBox->click();
	}

	// USB show dialog
	if (checkconfig(".config/22-usb/show-dialog") == true) {
		ui->usbmsDialogBox->click();
	}

	// Automatic updates
	if (checkconfig(".config/23-updates/check-updates") == true) {
		ui->autoCheckUpdatesBox->click();
	}

	// 'Export highlights' button
	if (!QFile::exists(global::localLibrary::highlightsDatabasePath) or readFile(global::localLibrary::highlightsDatabasePath).isEmpty()) {
		ui->exportHighlightsBtn->setEnabled(false);
	}

	if (readFile(".config/12-lockscreen/background") == "blank") {
		ui->lockscreenBackgroundComboBox->setCurrentIndex(0);
	} else if (readFile(".config/12-lockscreen/background") == "screenSaver") {
		ui->lockscreenBackgroundComboBox->setCurrentIndex(1);
	} else if (readFile(".config/12-lockscreen/background") == "background") {
		ui->lockscreenBackgroundComboBox->setCurrentIndex(2);
	}

	if (checkconfig("/opt/inkbox_genuine") == true) {
		// Enforcing security policy if the user has not rooted the device
		if (checkconfig("/external_root/opt/root/rooted") == true) {
			ui->usbNetworkingCheckBox->show();
			if (checkconfig("/external_root/boot/flags/USBNET_ENABLE")) {
				ui->usbNetworkingCheckBox->click();
			}
			ui->requestLeaseBtn->show();
			ui->label_4->show();
		} else {
			ui->usbNetworkingCheckBox->hide();
			ui->requestLeaseBtn->hide();
			ui->label_4->hide();
		}
		ui->enableLockscreenCheckBox->show();
		ui->setPasscodeBtn->show();
		ui->setPasscodeLabel->show();
		ui->usbmsBtn->show();
		ui->label_6->show();
	} else {
		ui->label_8->setText("Reset InkBox");
	}

	if (!device->supportsWifi()) {
		ui->checkOtaUpdateLabel->hide();
		ui->checkOtaUpdateBtn->hide();
		ui->checkOtaUpdateLabel->deleteLater();
		ui->checkOtaUpdateBtn->deleteLater();
		ui->checkOtaUpdateGridLayout->deleteLater();
	}

	UI::applyStyle(*this, "eink");
}

SettingsWidget::~SettingsWidget() {
	delete ui;
}

void SettingsWidget::on_okBtn_clicked() {
	saveDeferredSettings();

	// Prevent potential unknown damage launching via shell script this could do
	if (launch_sh == true) {
		if (ui_enable_changed == true) {
			ui_enable_changed = false;
			global::settings::settingsRebootDialog = true;
			generalDialogWindow = new GeneralDialog();
			generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
		} else {
			QProcess process;
			process.startDetached("inkbox.sh", QStringList());
			qApp->quit();
		}
	} else {
		QProcess process;
		process.startDetached("inkbox", QStringList());
		qApp->quit();
	}
}

void SettingsWidget::saveDeferredSettings() {
	// Save things
	writeFile(".config/07-words_number/config", QString::number(wordsNumberSaved));
	log("Set text files words number to " + QString::number(wordsNumberSaved), className);

	writeFile(".config/13-epub_page_size/width", QString::number(pageSizeWidthSaved));
	writeFile(".config/13-epub_page_size/set", "true");

	writeFile(".config/13-epub_page_size/height", QString::number(pageSizeHeightSaved));
	writeFile(".config/13-epub_page_size/set", "true");

	// Notify power daemon of a potential configuration update
	writeFile("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/updateConfig", "true");
}

void SettingsWidget::on_aboutBtn_clicked() {
	log("Showing About message box", className);
	if (checkconfig("/opt/inkbox_genuine") == true) {
		QString aboutmsg = "InkBox is an open-source, Qt-based eBook reader. It aims to bring you the latest Qt features while being also fast and responsive.";
		aboutmsg.prepend("<font face='u001'>");
		aboutmsg.append("<br><br>InkBox ");
		aboutmsg.append(readFile("/external_root/opt/isa/version"));
		aboutmsg.append("</font>");
		QMessageBox::information(this, tr("Information"), aboutmsg);
	} else {
		QMessageBox::information(this, tr("About"), tr("InkBox is an open-source Qt-based eBook reader. It aims to bring you the latest Qt features while being also fast and responsive."));
	}
}

void SettingsWidget::on_demoCheckBox_toggled(bool checked) {
	QString settingString = "change MainWindow label demo";
	// Write to config file
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/01-demo/config", "InkboxChangeLabel=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/01-demo/config", "InkboxChangeLabel=");
	}
}

void SettingsWidget::on_clockCheckBox_toggled(bool checked) {
	QString settingString = "clock with seconds";
	// Write to config file
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/02-clock/config", "ClockShowSeconds=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/02-clock/config", "ClockShowSeconds=");
	}
}

void SettingsWidget::on_quoteCheckBox_toggled(bool checked) {
	QString settingString = "disable show quotes";
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/05-quote/config", "DisableQuote=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/05-quote/config", "DisableQuote=");
	}
}

void SettingsWidget::on_requestLeaseBtn_clicked() {
	log("Requesting DHCP lease", className);
	QString prog("chroot");
	QStringList args;
	args << "/external_root"
		 << "/usr/sbin/dhclient";
	QProcess *proc = new QProcess();
	proc->start(prog, args);
	proc->waitForFinished();
	proc->deleteLater();
}

void SettingsWidget::on_usbmsBtn_clicked() {
	usbms_launch();
}

void SettingsWidget::on_updateBtn_clicked() {
	log("'Update' button clicked", className);
	writeFile("/mnt/onboard/onboard/.inkbox/can_really_update", "true\n");
	writeFile("/external_root/opt/update/will_update", "true\n");
	writeFile("/external_root/boot/flags/WILL_UPDATE", "true\n");
	QString prog("/sbin/reboot");
	QStringList args;
	QProcess *proc = new QProcess();
	proc->start(prog, args);
	proc->waitForFinished();
	proc->deleteLater();
}

void SettingsWidget::on_darkModeCheckBox_toggled(bool checked) {
	QString settingString = "dark mode";
	if (checked == true) {
		logEnabled(settingString, className);
		writeFile(".config/10-dark_mode/config", "true");
		writeFile("/tmp/invertScreen", "y");
	} else {
		logDisabled(settingString, className);
		writeFile(".config/10-dark_mode/config", "false");
		writeFile("/tmp/invertScreen", "n");
	}
}

void SettingsWidget::on_menuBarCheckBox_toggled(bool checked) {
	QString settingString = "sticky menu bar";
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/11-menubar/sticky", "StickyMenuBar=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/11-menubar/sticky", "StickyMenuBar=");
	}
}

void SettingsWidget::on_comboBox_currentIndexChanged(const QString &arg1) {
	log("Setting page refresh setting to " + arg1, className);
	if (arg1 == "Every page") {
		writeFile(".config/04-book/refresh", "0");
	}
	if (arg1 == "1 page") {
		writeFile(".config/04-book/refresh", "1");
	}
	if (arg1 == "2 pages") {
		writeFile(".config/04-book/refresh", "2");
	}
	if (arg1 == "3 pages") {
		writeFile(".config/04-book/refresh", "3");
	}
	if (arg1 == "4 pages") {
		writeFile(".config/04-book/refresh", "4");
	}
	if (arg1 == "5 pages") {
		writeFile(".config/04-book/refresh", "5");
	}
	if (arg1 == "6 pages") {
		writeFile(".config/04-book/refresh", "6");
	}
	if (arg1 == "Never refresh") {
		writeFile(".config/04-book/refresh", "-1");
	}
}

void SettingsWidget::on_resetBtn_clicked() {
	log("'Reset' button clicked", className);
	// We write to a temporary file to show a "Reset" prompt
	writeFile("/inkbox/resetDialog", "true");

	// We setup the dialog
	log("Showing reset dialog", className);
	generalDialogWindow = new GeneralDialog();
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void SettingsWidget::on_setPasscodeBtn_clicked() {
	log("'Set passcode' button clicked", className);
	log("Launching lockscreen binary", className);
	saveDeferredSettings();
	writeFile("/tmp/setPasscode", "true");
	QProcess process;
	process.startDetached("lockscreen", QStringList());
	qApp->quit();
}

void SettingsWidget::on_enableLockscreenCheckBox_toggled(bool checked) {
	QString settingString = "lockscreen";
	if (checked == true) {
		logEnabled(settingString, className);
		writeFile(".config/12-lockscreen/config", "true");
		// Launching passcode setup wizard if none seems to be set at the moment
		if (!(QFile::exists(".config/12-lockscreen/passcode") && QFile::exists(".config/12-lockscreen/salt"))) {
			ui->setPasscodeBtn->click();
		}
	} else {
		logDisabled(settingString, className);
		writeFile(".config/12-lockscreen/config", "false");
	}
}

void SettingsWidget::on_showSystemInfoBtn_clicked() {
	log("'Show system info' button clicked", className);
	global::usbms::usbmsDialog = false;

	Device::SysInfo info = Device::getSingleton()->getSystemInfo();

	QString text;
	text = "<b>InkBox OS version ";
	text.append(readFile("/external_root/opt/isa/version"));
	text.append("</b><br>Copyright <font face='Inter'>©</font> 2021-2024 Nicolas Mailloux and contributors<br>Special thanks to: Szybet, NiLuJe, akemnade, Rain92, Ansraer (GitHub)");
	text.append("<br><b>GUI Git commit:</b> ");
	text.append(info.inkGitVersion);
	text.append("<br><b>Device UID:</b> ");
	text.append(info.uid);
	text.append("<br><b>Kernel version:</b> ");
	text.append(info.kernelVersion);
	text.append("<br><b>Kernel build:</b> ");
	text.append(info.kernelBuild);
	text.append("<br><b>Kernel commit:</b> ");
	text.append(info.kernelCommit);
	text.append("<br><b>Device:</b> ");
	QString device = Device::getSingleton()->getID();
	text.append(device);
	text.append("<br><b>IP address: </b>");
	text.append(info.ipAddress);

	global::text::textBrowserContents = text;
	global::text::textBrowserDialog = true;

	// Show a system info dialog
	log("Showing system info dialog", className);
	generalDialogWindow = new GeneralDialog();
	generalDialogWindow->yIncrease = determineYIncrease();
	generalDialogWindow->increaseSize();
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void SettingsWidget::on_readerScrollBarCheckBox_toggled(bool checked) {
	QString settingString = "scrollbar display if necessary";
	if (checked == true) {
		logEnabled(settingString, className);
		writeFile(".config/14-reader_scrollbar/config", "true");
	} else {
		logDisabled(settingString, className);
		writeFile(".config/14-reader_scrollbar/config", "false");
	}
}

void SettingsWidget::brightnessDown() {
	UI::transitionBrightness(0);
}

void SettingsWidget::on_globalReadingSettingsCheckBox_toggled(bool checked) {
	QString settingString = "global reading settings";
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/16-global_reading_settings/config", "GlobalReadingSettings=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/16-global_reading_settings/config", "GlobalReadingSettings=");
	}
}

void SettingsWidget::on_checkOtaUpdateBtn_clicked() {
	log("'Check for OTA update' button clicked", className);
	if (testPing() == 0) {
		launchOtaUpdater();
	} else {
		emit showToast("Wi-Fi connection error");
	}
}

void SettingsWidget::openUpdateDialog() {
	log("Showing update dialog", className);
	global::mainwindow::updateDialog = true;
	// Write to a temporary file to show an "Update" prompt
	writeFile("/inkbox/updateDialog", "true");

	// Show the dialog
	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	connect(generalDialogWindow, SIGNAL(showToast(QString)), SLOT(showToastNative(QString)));
	connect(generalDialogWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToastNative()));
	QApplication::processEvents();
}

void SettingsWidget::launchOtaUpdater() {
	global::toast::modalToast = true;
	global::toast::indefiniteToast = true;
	emit showToast("Checking for updates");
	otaManagerWindow = new OTAUpdateManagerWidget(this);
	connect(otaManagerWindow, SIGNAL(canOtaUpdate(bool)), SLOT(openUpdateDialogOTA(bool)));
	otaManagerWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void SettingsWidget::openUpdateDialogOTA(bool open) {
	emit closeIndefiniteToast();
	if (open == true) {
		global::otaUpdate::isUpdateOta = true;
		openUpdateDialog();
	} else {
		emit showToast("No update available");
	}
}

void SettingsWidget::showToastNative(QString messageToDisplay) {
	emit showToast(messageToDisplay);
}

void SettingsWidget::closeIndefiniteToastNative() {
	emit closeIndefiniteToast();
}

void SettingsWidget::usbms_launch() {
	log("Showing USBMS splash", className);
	global::usbms::launchUsbms = true;

	usbmsWindow = new USBMSSplashWidget();
	usbmsWindow->setAttribute(Qt::WA_DeleteOnClose);
	usbmsWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
	usbmsWindow->show();
}

void SettingsWidget::quit_restart() {
	// If existing, cleaning bookconfig_mount mountpoint
	writeFile("/opt/ibxd", "bookconfig_unmount\n");

	// Restarting InkBox
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void SettingsWidget::on_enableEncryptedStorageCheckBox_toggled(bool checked) {
	QString settingString = "encrypted storage";
	if (checked == true) {
		if (enableEncryptedStorageUserChange == true) {
			logEnabled(settingString, className);
			setDefaultWorkDir();
			writeFile(".config/18-encrypted_storage/initial_setup_done", "false");
			writeFile(".config/18-encrypted_storage/status", "true");
			if (QFile::exists(".config/18-encrypted_storage/storage_list")) {
				QFile::remove(".config/18-encrypted_storage/storage_list");
			}
			global::settings::settingsRebootDialog = true;
			global::encfs::enableStorageEncryptionDialog = true;
			generalDialogWindow = new GeneralDialog(this);
			generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
		} else {
			enableEncryptedStorageUserChange = true;
		}
	} else {
		logDisabled(settingString, className);
		global::encfs::disableStorageEncryptionDialog = true;
		generalDialogWindow = new GeneralDialog(this);
		generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
		connect(generalDialogWindow, SIGNAL(cancelDisableStorageEncryption()), SLOT(cancelDisableStorageEncryption()));
		connect(generalDialogWindow, SIGNAL(disableStorageEncryption()), SLOT(disableStorageEncryption()));
	}
}

void SettingsWidget::disableStorageEncryption() {
	log("Disabling encrypted storage", className);
	setDefaultWorkDir();
	writeFile("/external_root/run/encfs_stop_cleanup", "true");
	writeFile("/opt/ibxd", "encfs_stop\n");
	QThread::msleep(5000);

	writeFile(".config/18-encrypted_storage/status", "false");
	QFile::remove(".config/18-encrypted_storage/initial_setup_done");
	QFile::remove(".config/18-encrypted_storage/storage_list");
	QFile::remove("/mnt/onboard/onboard/data.encfs");

	QDir dumpDir("/mnt/onboard/onboard/encfs-dropbox");
	dumpDir.removeRecursively();
	QDir decDir("/mnt/onboard/onboard/encfs-decrypted");
	decDir.removeRecursively();

	global::settings::settingsRebootDialog = true;
	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
}

void SettingsWidget::cancelDisableStorageEncryption() {
	enableEncryptedStorageUserChange = false;
	ui->enableEncryptedStorageCheckBox->click();
}

void SettingsWidget::on_repackBtn_clicked() {
	log("'Repack encrypted storage' button clicked", className);
	QDir dir("/mnt/onboard/onboard/encfs-dropbox");
	if (dir.isEmpty()) {
		log("Showing encrypted storage repack error dialog ('encfs-dropbox' directory is empty)", className);
		global::encfs::errorNoBooksInDropboxDialog = true;
		generalDialogWindow = new GeneralDialog(this);
		generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	} else {
		log("Showing encrypted storage repack dialog", className);
		writeFile("/external_root/run/encfs_repack", "true");
		quit_restart();
	}
}

void SettingsWidget::on_generateSystemReportBtn_clicked() {
	log("'Generate system report' button clicked", className);
	log("Generating system report", className);
	writeFile("/opt/ibxd", "generate_system_report\n");
	while (true) {
		if (QFile::exists("/inkbox/systemReportDone")) {
			if (checkconfig("/inkbox/systemReportDone") == true) {
				QFile::remove(global::localLibrary::databasePath);
				emit showToast("System report generated successfully");
			} else {
				emit showToast("Error in generating system report");
			}
			QFile::remove("/inkbox/systemReportDone");
			break;
		}
	}
}

void SettingsWidget::on_tzComboBox_currentTextChanged(const QString &arg1) {
	if (timezone_not_user_change == true) {
		timezone_not_user_change = false;
	} else {
		log("Setting timezone to " + arg1, className);
		setDefaultWorkDir();
		// Preventing unnecessary (e)MMC writes
		if (readFile(".config/19-timezone/config-name") != arg1) {
			QProcess::execute("ln", QStringList() << "-sf"
												  << "/usr/share/zoneinfo/" + arg1 << ".config/19-timezone/config");
			writeFile(".config/19-timezone/config-name", arg1);
			writeFile("/opt/ibxd", "gui_remount_localtime\n");
			QThread::msleep(500);
		}
	}
}

void SettingsWidget::on_exportHighlightsBtn_clicked() {
	QJsonObject jsonObject = readHighlightsDatabase();
	writeFile("/mnt/onboard/onboard/highlights-" + QDateTime::currentDateTime().toString("dd-MM-yy_hh.mm.ss") + ".json", QJsonDocument(jsonObject).toJson());
	showToast("Highlights exported successfully");
}

void SettingsWidget::on_wordsNumberIncBtn_clicked() {
	wordsNumberSaved = wordsNumberSaved + 5;
	ui->wordsNumberValueLabel->setText(QString::number(wordsNumberSaved));
}

void SettingsWidget::on_wordsNumberDecBtn_clicked() {
	if (wordsNumberSaved > 10) {
		wordsNumberSaved = wordsNumberSaved - 5;
		ui->wordsNumberValueLabel->setText(QString::number(wordsNumberSaved));
	}
}

void SettingsWidget::on_pageSizeHeightDecBtn_clicked() {
	if (pageSizeHeightSaved > 100) {
		pageSizeHeightSaved = pageSizeHeightSaved - 5;
		ui->pageSizeHeightLabel->setText(QString::number(pageSizeHeightSaved));
	}
}

void SettingsWidget::on_pageSizeHeightIncBtn_clicked() {
	pageSizeHeightSaved = pageSizeHeightSaved + 5;
	ui->pageSizeHeightLabel->setText(QString::number(pageSizeHeightSaved));
}

void SettingsWidget::on_pageSizeWidthDecBtn_clicked() {
	if (pageSizeWidthSaved > 100) {
		pageSizeWidthSaved = pageSizeWidthSaved - 5;
		ui->pageSizeWidthLabel->setText(QString::number(pageSizeWidthSaved));
	}
}

void SettingsWidget::on_pageSizeWidthIncBtn_clicked() {
	pageSizeWidthSaved = pageSizeWidthSaved + 5;
	ui->pageSizeWidthLabel->setText(QString::number(pageSizeWidthSaved));
}

void SettingsWidget::on_localLibraryShowFoldersCheckBox_toggled(bool checked) {
	QString settingString = "local library folders support";
	if (checked == true) {
		logEnabled(settingString, className);
		checked_box = true;
		writeconfig(".config/21-local_library/folders", "Folders=");
	} else {
		logDisabled(settingString, className);
		checked_box = false;
		writeconfig(".config/21-local_library/folders", "Folders=");
	}
}

void SettingsWidget::on_usbmsDialogBox_clicked(bool checked) {
	QString settingString = "USB Mass Storage dialog display";
	if (checked == true) {
		logEnabled(settingString, className);
		writeFile(".config/22-usb/show-dialog", "true");
	} else {
		logDisabled(settingString, className);
		writeFile(".config/22-usb/show-dialog", "false");
	}
}

void SettingsWidget::on_autoCheckUpdatesBox_clicked(bool checked) {
	QString settingString = "automatic updates checking";
	if (checked == true) {
		logEnabled(settingString, className);
		writeFile(".config/23-updates/check-updates", "true");
	} else {
		logDisabled(settingString, className);
		writeFile(".config/23-updates/check-updates", "false");
	}
}

void SettingsWidget::on_lockscreenBackgroundComboBox_currentTextChanged(const QString &arg1) {
	if (arg1 == "Blank") {
		writeFile(".config/12-lockscreen/background", "blank");
		log("Set lockscreen background to 'blank'", className);
	} else if (arg1 == "Screensaver picture") {
		writeFile(".config/12-lockscreen/background", "screenSaver");
		log("Set lockscreen background to 'screenSaver'", className);
	} else if (arg1 == "Device's screen") {
		writeFile(".config/12-lockscreen/background", "background");
		log("Set lockscreen background to 'background'", className);
	}
}

void SettingsWidget::on_settingsStackedWidget_currentChanged(int arg1) {
	if (ui->settingsStackedWidget->currentIndex() != 0) {
		ui->headerLabel->setText(ui->settingsStackedWidget->currentWidget()->objectName());
		ui->headerStackedWidget->show();
	} else {
		ui->headerStackedWidget->hide();
	}
}

void SettingsWidget::on_readingSettingsBtn_clicked() {
	log("'Reading settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(1);
}

void SettingsWidget::on_homeSettingsBtn_clicked() {
	log("'Home settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(2);
}

void SettingsWidget::on_librarySettingsBtn_clicked() {
	log("'Library settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(3);
}

void SettingsWidget::on_storageSettingsBtn_clicked() {
	log("'Storage settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(4);
}

void SettingsWidget::on_systemSettingsBtn_clicked() {
	log("'System settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(5);
}

void SettingsWidget::on_securitySettingsBtn_clicked() {
	log("'Security settings' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(6);
}

void SettingsWidget::on_headerBtn_clicked() {
	// "Home" button
	log("'Home' button clicked", className);
	ui->settingsStackedWidget->setCurrentIndex(0);
}

void SettingsWidget::on_usbNetworkingCheckBox_toggled(bool checked) {
	if (checked) {
		writeFile("/external_root/boot/flags/USBNET_ENABLE", "true");
		writeFile("/opt/ibxd", "usbnet_start\n");
	} else {
		writeFile("/external_root/boot/flags/USBNET_ENABLE", "false");
		writeFile("/opt/ibxd", "usbnet_stop\n");
	}
}
