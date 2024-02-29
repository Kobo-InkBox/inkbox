#include "generaldialog.h"
#include "device.h"
#include "functions.h"
#include "mainwindow.h"
#include "reader.h"
#include "ui.h"
#include "ui_generaldialog.h"

#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QListView>
#include <QProcess>
#include <QScreen>
#include <QStringListModel>
#include <QTextEdit>
#include <QTimer>

GeneralDialog::GeneralDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::GeneralDialog) {
	Device *device = Device::getSingleton();

	ui->setupUi(this);
	ui->bodyLabel->setFont(QFont("u001"));
	ui->searchComboBox->setFont(QFont("u001"));

	// Preventing outside interaction
	this->setModal(true);

	// Stylesheet, style & misc.
	if (global::keyboard::encfsDialog == true) {
		UI::applyStyle(*this, ":/resources/eink-square-encfs.qss");
	} else {
		UI::applyStyle(*this, "eink");
	}

	ui->okBtn->setProperty("type", "borderless");
	ui->cancelBtn->setProperty("type", "borderless");
	ui->acceptBtn->setProperty("type", "borderless");

	ui->okBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
	ui->cancelBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
	ui->acceptBtn->setStyleSheet("font-size: 9pt; padding: 10px; font-weight: bold; background: lightGrey");
	ui->headerLabel->setStyleSheet("font-weight: bold");
	ui->bodyLabel->setStyleSheet("font-size: 9.5pt");
	ui->searchComboBox->setStyleSheet("font-size: 9pt");

	// Disabling "Online library" search if device doesn't have Wi-Fi
	if (device->supportsWifi()) {
		ui->searchComboBox->removeItem(2);
	}

	if (QFile::exists("/inkbox/searchComboBoxFunction") == true) {
		QString searchComboBoxFunction = readFile("/inkbox/searchComboBoxFunction");
		if (searchComboBoxFunction == "Dictionary") {
			ui->searchComboBox->setCurrentIndex(0);
		} else if (searchComboBoxFunction == "Local storage") {
			ui->searchComboBox->setCurrentIndex(1);
		} else if (searchComboBoxFunction == "Online library") {
			ui->searchComboBox->setCurrentIndex(2);
		} else {
			ui->searchComboBox->setCurrentIndex(0);
		}
	}

	if (checkconfig("/inkbox/resetDialog") == true) {
		if (checkconfig("/opt/inkbox_genuine") == true) {
			resetDialog = true;
			ui->okBtn->setText("Proceed");
			ui->cancelBtn->setText("Go back");
			ui->bodyLabel->setText("This will erase any books\nyou have stored on the device.\nSettings will be reset.");

			ui->headerLabel->setText("Warning");
			QTimer::singleShot(50, this, SLOT(adjust_size()));
			writeFile("/inkbox/resetDialog", "false");
		} else {
			resetDialog = true;
			ui->okBtn->setText("Proceed");
			ui->cancelBtn->setText("Go back");
			ui->bodyLabel->setText("Settings will be reset.");
			ui->headerLabel->setText("Warning");
			QTimer::singleShot(50, this, SLOT(adjust_size()));
			writeFile("/inkbox/resetDialog", "false");
		}
	} else if (checkconfig("/inkbox/updateDialog") == true) {
		updateDialog = true;
		ui->okBtn->setText("Update");
		ui->cancelBtn->setText("Not now");
		ui->bodyLabel->setText("<font face='u001'>Do you want to update InkBox now</font><font face='Inter'>?</font>"); // Because I hate Univers/U001's question mark ...
		ui->headerLabel->setText("Update available");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
		writeFile("/inkbox/updateDialog", "false");
	} else if (global::settings::settingsRebootDialog == true) {
		settingsRebootDialog = true;
		ui->stackedWidget->setCurrentIndex(1);
		if (global::kobox::koboxSettingsRebootDialog == true or global::encfs::enableStorageEncryptionDialog) {
			koboxSettingsRebootDialog = true;
			ui->bodyLabel->setText("The device will reboot now, since<br>the settings you defined require it to work properly.");
		} else {
			ui->bodyLabel->setText("The settings you defined might<br>require a complete reboot of the device for them to work properly.");
		}
		ui->headerLabel->setText("Information");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::mainwindow::lowBatteryDialog == true) {
		lowBatteryDialog = true;
		ui->stackedWidget->setCurrentIndex(1);
		QString message = "The battery's level is low.\nPlease charge your eReader.\nCurrent level: ";
		message.append(device->getBatteryLevel());
		ui->bodyLabel->setText(message);
		ui->headerLabel->setText("Low battery");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
		writeFile("/inkbox/lowBatteryDialog", "false");
	} else if (global::usbms::usbmsDialog == true) {
		usbmsDialog = true;
		ui->stackedWidget->setCurrentIndex(0);
		ui->okBtn->setText("Connect");
		ui->cancelBtn->setText("Not now");
		ui->bodyLabel->setText("<font face='u001'>Do you want to connect your device to a computer to manage books</font><font face='Inter'>?</font>");
		ui->headerLabel->setText("USB cable connected");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::text::textBrowserDialog == true) {
		textBrowserDialog = true;
		textwidgetWindow = new textwidget();

		if (global::text::textBrowserTitle.isEmpty() == true) {
			ui->headerLabel->setText("Information");
		} else {
			ui->headerLabel->setText(global::text::textBrowserTitle);
		}
		// Important
		ui->bodyLabel->setText(global::text::textBrowserContents);

		ui->stackedWidget->setCurrentIndex(1);
		ui->mainStackedWidget->insertWidget(1, textwidgetWindow);
		ui->mainStackedWidget->setCurrentIndex(1);
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::kobox::resetKoboxDialog == true) {
		resetKoboxDialog = true;
		ui->headerLabel->setText("Warning");
		ui->okBtn->setText("Proceed");
		ui->cancelBtn->setText("Go back");
		ui->bodyLabel->setText("This will erase all KoBox user data. KoBox settings will be reset.");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::keyboard::keyboardDialog == true) {
		setupKeyboardDialog();
	} else if (global::keyboard::keypadDialog == true) {
		keypadDialog = true;
		keypadWidget = new virtualkeypad();
		ui->headerLabel->setText("Enter a number");
		ui->okBtn->setText("OK");
		ui->cancelBtn->setText("Cancel");
		ui->mainStackedWidget->insertWidget(1, keypadWidget);
		ui->mainStackedWidget->setCurrentIndex(1);
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::encfs::disableStorageEncryptionDialog == true) {
		ui->headerLabel->setText("Warning");
		ui->okBtn->setText("Proceed");
		ui->cancelBtn->setText("Go back");
		ui->bodyLabel->setText("<font face='u001'>This will delete all the files you have encrypted. Are you sure you want to continue</font><font face='Inter'>?</font>");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::encfs::errorNoBooksInDropboxDialog == true) {
		ui->stackedWidget->setCurrentIndex(1);
		ui->headerLabel->setText("Error");
		ui->bodyLabel->setText("Please put books in the 'encfs-dropbox' folder to repack your encrypted storage.");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::encfs::repackDialog == true) {
		ui->headerLabel->setText("Information");
		ui->okBtn->setText("Proceed");
		ui->cancelBtn->setText("Not now");
		ui->bodyLabel->setText("<font face='u001'>New files have been found in 'encfs-dropbox'. Would you want to repack your encrypted storage</font><font face='Inter'>?</font>");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::userApps::appCompatibilityDialog == true) {
		appCompatibilityDialog = true;
		global::userApps::appCompatibilityLastContinueStatus = true;
		ui->okBtn->setText("Continue");
		ui->cancelBtn->setText("Cancel");
		ui->bodyLabel->setText(global::userApps::appCompatibilityText);
		ui->headerLabel->setText("Compatibility warning");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::userApps::appInfoDialog == true) {
		appInfoDialog = true;
		textwidgetWindow = new textwidget();

		ui->headerLabel->setText("App info");
		ui->stackedWidget->setCurrentIndex(1);
		ui->mainStackedWidget->insertWidget(1, textwidgetWindow);
		ui->mainStackedWidget->setCurrentIndex(1);
		yIncrease = 1.8;
		QTimer::singleShot(50, this, SLOT(increaseSize()));
	} else if (global::library::librarySyncDialog == true) {
		librarySyncDialog = true;
		ui->okBtn->setText("Continue");
		ui->cancelBtn->setText("Not now");
		ui->bodyLabel->setText("<font face='u001'>Online library requires syncing. Do you want to continue</font><font face='Inter'>?</font>");
		ui->headerLabel->setText("Sync required");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else if (global::telemetry::telemetryDialog == true) {
		telemetryDialog = true;
		ui->stackedWidget->setCurrentIndex(0);
		ui->okBtn->setText("Send");
		ui->cancelBtn->setText("Don't send");
		ui->bodyLabel->setText("<font face='u001'>We, the InkBox OS developers, would like to know a bit more about our userbase.<br>We would be extremely grateful if you would allow us to collect some information about your device.<br>Would you like to send it to us</font><font face='Inter'>?</font><font face='u001'><br>No personal data will be transmitted.</font>");
		ui->headerLabel->setText("Telemetry request");
		QTimer::singleShot(50, this, SLOT(adjust_size()));
	} else {
		// We shouldn't be there ;)
		log("Launched without settings", className);
	}
}

GeneralDialog::~GeneralDialog() {
	delete ui;
}

void GeneralDialog::on_cancelBtn_clicked() {
	log("Cancel button clicked", className);
	if (updateDialog == true) {
		writeFile("/tmp/cancelUpdateDialog", "true");
		GeneralDialog::close();
	} else if (usbmsDialog == true) {
		global::usbms::usbmsDialog = false;
		GeneralDialog::close();
	} else if (resetKoboxDialog == true) {
		global::kobox::resetKoboxDialog = false;
		GeneralDialog::close();
	} else if (keypadDialog == true) {
		global::keyboard::keypadDialog = false;
		GeneralDialog::close();
	} else if (global::encfs::disableStorageEncryptionDialog == true) {
		emit cancelDisableStorageEncryption();
		global::encfs::disableStorageEncryptionDialog = false;
		GeneralDialog::close();
	} else {
		if (global::keyboard::searchDialog == true) {
			global::keyboard::searchDialog = false;
			global::forbidOpenSearchDialog = true;
			global::keyboard::keyboardDialog = false;
			global::keyboard::keyboardText = "";
			global::library::librarySearchDialog = false;
		} else if (global::keyboard::vncDialog == true) {
			global::keyboard::vncDialog = false;
			global::keyboard::keyboardDialog = false;
			global::keyboard::keyboardText = "";
		} else if (global::keyboard::encfsDialog == true) {
			global::keyboard::encfsDialog = false;
			global::encfs::cancelSetup = true;
			global::keyboard::keyboardText = "";
		} else if (global::encfs::errorNoBooksInDropboxDialog == true) {
			global::encfs::errorNoBooksInDropboxDialog = false;
		} else if (global::encfs::repackDialog == true) {
			global::encfs::repackDialog = false;
		} else if (global::userApps::appCompatibilityDialog == true) {
			global::userApps::launchApp = false;
			global::userApps::appCompatibilityLastContinueStatus = false;
			global::userApps::appCompatibilityText = "";
			global::userApps::appCompatibilityDialog = false;
		} else if (global::library::librarySyncDialog == true) {
			emit noSyncOnlineLibrary();
			global::library::librarySyncDialog = false;
		} else if (global::telemetry::telemetryDialog == true) {
			global::telemetry::telemetryDialog = false;
			log("User declined telemetry request", className);
			writeFile("/mnt/onboard/.adds/inkbox/.config/24-telemetry/asked", "true");
		} else if (global::keyboard::telemetryMessageDialog == true) {
			global::keyboard::telemetryMessageDialog = false;
			emit telemetryMessage(global::keyboard::keyboardText);
			global::keyboard::keyboardText = "";
			log("No message to be sent along with collected data", className);
		}
		GeneralDialog::close();
	}
}

void GeneralDialog::on_okBtn_clicked() {
	log("OK button clicked", className);
	if (resetDialog == true) {
		if (checkconfig("/opt/inkbox_genuine") == true) {
			// Soft-reset the device
			// We set a custom boot flag and reboot silently in Diagnostics
			log("Setting up the device for soft-reset", className);
			writeFile("/external_root/boot/flags/DIAGS_BOOT", "true\n");
			writeFile("/external_root/boot/flags/DO_SOFT_RESET", "true\n");
			reboot(false);
			qApp->quit();
		} else {
			// Restore default settings, we're not on InkBox OS
			log("Restoring default settings", className);
			QString prog("sh");
			QStringList args;
			args << "reset-config.sh";
			QProcess *proc = new QProcess();
			proc->start(prog, args);
			proc->waitForFinished();
			proc->deleteLater();

			// Relaunching InkBox
			QProcess process;
			process.startDetached("inkbox.sh", QStringList());
			qApp->quit();
		}
	}
	if (updateDialog == true) {
		if (global::otaUpdate::isUpdateOta == true) {
			this->hide();

			global::otaUpdate::downloadOta = true;
			otaManagerWindow = new OTAUpdateManagerWidget(this);
			connect(otaManagerWindow, SIGNAL(downloadedOtaUpdate(bool)), SLOT(startOtaUpdate(bool)));
			otaManagerWindow->setAttribute(Qt::WA_DeleteOnClose);

			global::toast::indefiniteToast = true;
			global::toast::modalToast = true;
			emit showToast("Downloading update");
		} else {
			installUpdate();
			qApp->quit();
		}
	}
	if (usbmsDialog == true) {
		log("Showing USBMS splash", className);
		global::usbms::usbmsDialog = false;
		global::usbms::launchUsbms = true;

		usbmsWindow = new USBMSSplashWidget();
		usbmsWindow->setAttribute(Qt::WA_DeleteOnClose);
		usbmsWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
		usbmsWindow->show();
	}
	if (resetKoboxDialog == true) {
		resetKoboxUserData();
	}
	if (keypadDialog == true) {
		int number = global::keyboard::keypadText.toInt();
		emit gotoPageSelected(number);
		global::keyboard::keypadDialog = false;
		GeneralDialog::close();
	}
	if (keyboardDialog == true) {
		if (global::keyboard::searchDialog == true) {
			if (!global::keyboard::keyboardText.isEmpty()) {
				if (ui->searchComboBox->currentText() == "Dictionary") {
					writeFile("/inkbox/searchComboBoxFunction", "Dictionary");
					for (int i = ui->mainStackedWidget->count(); i >= 0; i--) {
						QWidget *widget = ui->mainStackedWidget->widget(i);
						ui->mainStackedWidget->removeWidget(widget);
						widget->deleteLater();
					}
					ui->topStackedWidget->setVisible(false);
					ui->stackedWidget->setVisible(false);
					dictionaryWidgetWindow = new dictionaryWidget();
					dictionaryWidgetWindow->setAttribute(Qt::WA_DeleteOnClose);
					connect(dictionaryWidgetWindow, SIGNAL(refreshScreen()), SLOT(refreshScreenNative()));
					connect(dictionaryWidgetWindow, SIGNAL(destroyed(QObject *)), SLOT(restartSearchDialog()));
					ui->mainStackedWidget->insertWidget(1, dictionaryWidgetWindow);
				} else if (ui->searchComboBox->currentText() == "Local storage") {
					writeFile("/inkbox/searchComboBoxFunction", "Local storage");
					QString onboardPath;
					QStringList storageSearchResults;
					if (checkconfig("/opt/inkbox_genuine") == true) {
						onboardPath = "/mnt/onboard/onboard/";
					} else {
						onboardPath = "/mnt/onboard/";
					}
					QDirIterator dirIt(onboardPath, QDirIterator::Subdirectories);
					log("Searching local storage for '" + global::keyboard::keyboardText + "'", className);
					while (dirIt.hasNext()) {
						dirIt.next();
						if (QFileInfo(dirIt.filePath()).isFile()) {
							QString suffix = QFileInfo(dirIt.filePath()).suffix();
							if (suffix == "txt" or suffix == "TXT" or suffix == "epub" or suffix == "EPUB" or suffix == "pdf" or suffix == "PDF" or suffix == "png" or suffix == "PNG" or suffix == "tif" or suffix == "TIF" or suffix == "bmp" or suffix == "BMP" or suffix == "tiff" or suffix == "TIFF" or suffix == "jpg" or suffix == "JPG" or suffix == "jpeg" or suffix == "JPEG") {
								if (dirIt.fileName().contains(global::keyboard::keyboardText) == true) {
									storageSearchResults.append(dirIt.fileName());
									global::localStorage::searchResultsPaths.append(dirIt.filePath());
								}
							}
						}
					}
					if (!storageSearchResults.isEmpty()) {
						log("Displaying search results", className);
						for (int i = ui->mainStackedWidget->count(); i >= 0; i--) {
							QWidget *widget = ui->mainStackedWidget->widget(i);
							ui->mainStackedWidget->removeWidget(widget);
							widget->deleteLater();
						}
						ui->topStackedWidget->setVisible(false);
						ui->stackedWidget->setVisible(false);
						searchResultsWidgetWindow = new searchResultsWidget(this);
						searchResultsWidgetWindow->setAttribute(Qt::WA_DeleteOnClose);
						global::forbidOpenSearchDialog = true;
						connect(searchResultsWidgetWindow, SIGNAL(destroyed(QObject *)), SLOT(restartSearchDialog()));
						connect(searchResultsWidgetWindow, SIGNAL(openBookFile(QString, bool)), SLOT(openBookFileNative(QString, bool)));
						searchResultsWidgetWindow->setListViewContents(storageSearchResults);
						ui->mainStackedWidget->insertWidget(1, searchResultsWidgetWindow);
					} else {
						log("No search results found", className);
						global::toast::delay = 3000;
						emit showToast("No results found");
						keyboardWidget->clearLineEdit();
						global::keyboard::keyboardText = "";
					}
				} else if (ui->searchComboBox->currentText() == "Online library") {
					if (Device::getSingleton()->supportsWifi()) {
						writeFile("/inkbox/searchComboBoxFunction", "Online library");
						log("Searching online library for '" + global::keyboard::keyboardText + "'", className);

						if (!readFile("/external_root/opt/storage/gutenberg/last_sync").isEmpty()) {
							unsigned long currentEpoch = QDateTime::currentSecsSinceEpoch();
							unsigned long syncEpoch = readFile("/external_root/opt/storage/gutenberg/last_sync").toULong();
							unsigned long allowSyncEpoch = syncEpoch + 86400;
							if (currentEpoch > allowSyncEpoch) {
								syncGutenbergCatalog();
							} else {
								noGutenbergSyncToDo = true;
							}
						} else {
							syncGutenbergCatalog();
						}

						QTimer *searchTimer = new QTimer(this);
						searchTimer->setInterval(100);
						connect(searchTimer, &QTimer::timeout, [&]() {
							if (noGutenbergSyncToDo == true or (gutenbergSyncDone == true && gutenbergSyncStatus == true)) {
								if (searchTimerDone == false) {
									searchTimerDone = true;
									writeFile("/inkbox/gutenberg_search_request", global::keyboard::keyboardText);
									writeFile("/opt/ibxd", "gutenberg_search\n");
									global::toast::modalToast = true;
									global::toast::indefiniteToast = true;
									emit showToast("Searching");
									QTimer::singleShot(100, this, SLOT(waitForGutenbergSearchDone()));
								}
							}
						});
						searchTimer->start();
					} else {
						emit showToast("Wi-Fi connection error");
					}
				}
			} else {
				global::toast::delay = 3000;
				emit showToast("Please type in a search term");
			}
		} else if (global::keyboard::vncDialog == true) {
			if (!global::keyboard::keyboardText.isEmpty()) {
				log("Gathering VNC connection information", className);
				if (vncServerSet != true) {
					vncServerAddress = global::keyboard::keyboardText;
					vncServerSet = true;
					keyboardWidget->clearLineEdit();
					ui->headerLabel->setText("Enter the server's password");
				} else if (vncPasswordSet != true) {
					vncServerPassword = global::keyboard::keyboardText;
					vncPasswordSet = true;
					keyboardWidget->clearLineEdit();
					ui->headerLabel->setText("Enter the server's port");
				} else {
					vncServerPort = global::keyboard::keyboardText;
					global::keyboard::vncDialog = false;
					global::keyboard::keyboardDialog = false;
					startVNC(vncServerAddress, vncServerPassword, vncServerPort);
					GeneralDialog::close();
				}
			} else {
				global::toast::delay = 3000;
				emit showToast("Please type in the required argument");
			}
		} else if (global::keyboard::wifiPassphraseDialog == true) {
			if (!global::keyboard::keyboardText.isEmpty()) {
				if (global::keyboard::keyboardText.count() < 8) {
					global::toast::delay = 3000;
					showToast("Minimum passphrase length is 8 characters");
				} else {
					GeneralDialog::close();
				}
			} else {
				global::toast::delay = 3000;
				showToast("Please type in the required argument");
			}
		} else if (global::keyboard::encfsDialog == true) {
			if (!global::keyboard::keyboardText.isEmpty()) {
				global::encfs::passphrase = global::keyboard::keyboardText;
				global::keyboard::encfsDialog = false;
				global::keyboard::keyboardText = "";
				global::keyboard::keyboardDialog = false;
				this->close();
			} else {
				global::toast::delay = 3000;
				showToast("Please type in the required argument");
			}
		} else if (global::keyboard::telemetryMessageDialog == true) {
			if (!global::keyboard::keyboardText.isEmpty()) {
				emit telemetryMessage(global::keyboard::keyboardText);
				log("Message to be sent along with collected data is: " + global::keyboard::keyboardText, className);
				global::keyboard::keyboardText = "";
				global::keyboard::telemetryMessageDialog = false;
				global::keyboard::keyboardDialog = false;
				this->close();
			} else {
				global::toast::delay = 3000;
				showToast("Please type in the required argument");
			}
		} else {
			global::keyboard::keyboardDialog = false;
			GeneralDialog::close();
		}
	}
	if (global::encfs::disableStorageEncryptionDialog == true) {
		global::encfs::disableStorageEncryptionDialog = false;
		emit disableStorageEncryption();
		this->close();
	}
	if (global::encfs::repackDialog == true) {
		global::encfs::repackDialog = false;
		log("Encrypted storage repack requested", className);
		writeFile("/external_root/run/encfs_repack", "true");
		quit_restart();
	} else if (global::userApps::appCompatibilityDialog == true) {
		global::userApps::launchApp = true;
		global::userApps::appCompatibilityText = "";
		global::userApps::appCompatibilityLastContinueStatus = true; // Not really necessary, only needed if something fails horribly
		global::userApps::appCompatibilityDialog = false;
		GeneralDialog::close();
	} else if (global::library::librarySyncDialog == true) {
		emit syncOnlineLibrary();
		global::library::librarySyncDialog = false;
		GeneralDialog::close();
	} else if (global::telemetry::telemetryDialog == true) {
		global::telemetry::telemetryDialog = false;
		global::keyboard::telemetryMessageDialog = true;
		GeneralDialog::close();
	}
}
void GeneralDialog::on_acceptBtn_clicked() {
	log("OK button clicked", className);
	if (lowBatteryDialog == true) {
		global::mainwindow::lowBatteryDialog = false;
		global::battery::batteryAlertLock = false;
		global::battery::showLowBatteryDialog = false;
	}

	if (settingsRebootDialog == true) {
		if (koboxSettingsRebootDialog == true or global::encfs::enableStorageEncryptionDialog) {
			reboot(true);
			qApp->quit();
		} else {
			QProcess process;
			process.startDetached("inkbox.sh", QStringList());
			qApp->quit();
		}
	}
	if (textBrowserDialog == true) {
		global::text::textBrowserContents = "";
		global::text::textBrowserTitle = "";
		global::text::textBrowserDialog = false;
	}

	if (appInfoDialog == true) {
		global::text::textBrowserContents = "";
		global::text::textBrowserTitle = "";
		global::userApps::appInfoDialog = false;
	}

	// We don't have any other option ;p
	GeneralDialog::close();
}

void GeneralDialog::adjust_size() {
	float widthProportion = 2;
	float heightProportion = 2;
	if (telemetryDialog) {
		Device::ScreenSize screenSize = Device::getSingleton()->getScreenSize();
		if (screenSize == Device::ScreenSize::SMALL) {
			widthProportion = 3.3;
			heightProportion = 2.6;
		} else if (screenSize == Device::ScreenSize::MEDIUM) {
			widthProportion = 3.4;
			heightProportion = 2.6;
		} else if (screenSize == Device::ScreenSize::LARGE) {
			widthProportion = 3.4;
			heightProportion = 2.6;
		} else if (screenSize == Device::ScreenSize::VERY_LARGE) {
			widthProportion = 3;
			heightProportion = 2.6;
		}
	}

	this->adjustSize();
	QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
	int x = (screenGeometry.width() - this->width()) / widthProportion;
	int y = (screenGeometry.height() - this->height()) / heightProportion;
	this->move(x, y);
	this->show();
	emit refreshScreen();
}

void GeneralDialog::restartSearchDialog() {
	GeneralDialog::close();
}

void GeneralDialog::setupKeyboardDialog() {
	adjust_size();
	ui->stackedWidget->setCurrentIndex(0);
	keyboardDialog = true;
	ui->stackedWidget->setVisible(true);
	if (global::keyboard::searchDialog == true) {
		ui->topStackedWidget->setCurrentIndex(1);
		ui->searchHeaderLabel->setText("Search");
		ui->okBtn->setText("Search");
		ui->cancelBtn->setText("Close");
	} else if (global::keyboard::vncDialog == true) {
		ui->headerLabel->setText("Enter the server's IP address");
		ui->okBtn->setText("OK");
		ui->cancelBtn->setText("Cancel");
	} else if (global::keyboard::wifiPassphraseDialog == true) {
		ui->headerLabel->setText("Enter the network's passphrase");
		ui->okBtn->setText("Enter");
		ui->cancelBtn->setText("Cancel");
	} else if (global::keyboard::encfsDialog == true) {
		ui->headerLabel->setText("Enter your encrypted storage's passphrase");
		ui->okBtn->setText("OK");
		ui->cancelBtn->setText("Cancel");
	} else if (global::keyboard::telemetryMessageDialog == true) {
		ui->headerLabel->setWordWrap(true);
		ui->headerLabel->setText("If you'd like, feel free to send us a random message along with the collected data");
		ui->okBtn->setText("Continue");
		ui->cancelBtn->setText("No, thanks");
	} else {
		ui->headerLabel->setText("Enter a string");
		ui->okBtn->setText("OK");
		ui->cancelBtn->setText("Cancel");
	}
	keyboardWidget = new virtualkeyboard();
	connect(keyboardWidget, SIGNAL(adjust_size()), SLOT(adjust_size()));
	ui->mainStackedWidget->insertWidget(1, keyboardWidget);
	ui->mainStackedWidget->setCurrentIndex(1);
	adjust_size();
	QTimer::singleShot(1000, this, SLOT(adjust_size()));
}

void GeneralDialog::refreshScreenNative() {
	emit refreshScreen();
}

void GeneralDialog::startVNC(QString server, QString password, QString port) {
	log("Launching VNC viewer", className);
	writeFile("/external_root/tmp/app_vnc_server", server);
	writeFile("/external_root/tmp/app_vnc_password", password);
	writeFile("/external_root/tmp/app_vnc_port", port);
	writeFile("/opt/ibxd", "app_start_vnc\n");
	qApp->quit();
}

void GeneralDialog::startOtaUpdate(bool wasDownloadSuccessful) {
	emit closeIndefiniteToast();
	if (wasDownloadSuccessful == true) {
		global::otaUpdate::isUpdateOta = false;
		installUpdate();
		qApp->quit();
	} else {
		emit showToast("Download failed");
		global::otaUpdate::isUpdateOta = false;
	}
	GeneralDialog::close();
}

void GeneralDialog::openBookFileNative(QString book, bool relativePath) {
	emit openBookFile(book, relativePath);
}

void GeneralDialog::showToastNative(QString messageToDisplay) {
	emit showToast(messageToDisplay);
}

void GeneralDialog::closeIndefiniteToastNative() {
	emit closeIndefiniteToast();
}

void GeneralDialog::quit_restart() {
	log("Restarting InkBox", className);
	// If existing, cleaning bookconfig_mount mountpoint
	writeFile("/opt/ibxd", "bookconfig_unmount\n");

	// Restarting InkBox
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void GeneralDialog::syncGutenbergCatalog() {
	log("Syncing Gutenberg catalog", className);
	global::toast::modalToast = true;
	global::toast::indefiniteToast = true;
	emit showToast("Sync in progress");

	writeFile("/opt/ibxd", "gutenberg_sync\n");
	QTimer *syncCheckTimer = new QTimer(this);
	syncCheckTimer->setInterval(500);
	connect(syncCheckTimer, &QTimer::timeout, [&]() {
		if (QFile::exists("/inkbox/gutenbergSyncDone") == true) {
			if (checkconfig("/inkbox/gutenbergSyncDone") == true) {
				log("Gutenberg sync successfully completed", className);
				gutenbergSyncDone = true;
				gutenbergSyncStatus = true;
				emit closeIndefiniteToast();
			} else {
				log("Gutenberg sync encountered an error", className);
				gutenbergSyncDone = true;
				gutenbergSyncStatus = false;
				emit closeIndefiniteToast();
				emit showToast("Error");
				global::keyboard::searchDialog = false;
				global::forbidOpenSearchDialog = true;
				global::keyboard::keyboardDialog = false;
				global::keyboard::keyboardText = "";
				global::library::librarySearchDialog = false;
				QTimer::singleShot(5000, this, SLOT(close()));
			}
			QFile::remove("/inkbox/gutenbergSyncDone");
		}
	});
	syncCheckTimer->start();
}

void GeneralDialog::waitForGutenbergSearchDone() {
	while (true) {
		if (QFile::exists("/inkbox/gutenberg-search/search_done")) {
			if (checkconfig("/inkbox/gutenberg-search/search_done") == true) {
				QList<QString> searchResults = readFile("/inkbox/gutenberg-search/search_results_titles").split("\n");
				searchResults.takeLast();
				global::library::libraryResults = true;

				for (int i = ui->mainStackedWidget->count(); i >= 0; i--) {
					QWidget *widget = ui->mainStackedWidget->widget(i);
					ui->mainStackedWidget->removeWidget(widget);
					widget->deleteLater();
				}
				ui->topStackedWidget->setVisible(false);
				ui->stackedWidget->setVisible(false);
				searchResultsWidgetWindow = new searchResultsWidget(this);
				searchResultsWidgetWindow->setAttribute(Qt::WA_DeleteOnClose);
				global::forbidOpenSearchDialog = true;
				connect(searchResultsWidgetWindow, SIGNAL(destroyed(QObject *)), SLOT(restartSearchDialog()));
				connect(searchResultsWidgetWindow, SIGNAL(showToast(QString)), SLOT(showToastNative(QString)));
				connect(searchResultsWidgetWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToastNative()));
				connect(searchResultsWidgetWindow, SIGNAL(hideDialog()), SLOT(hide()));
				searchResultsWidgetWindow->setListViewContents(searchResults);
				ui->mainStackedWidget->insertWidget(1, searchResultsWidgetWindow);
				QFile::remove("/inkbox/gutenberg-search/search_done");
				emit closeIndefiniteToast();
				break;
			} else {
				emit closeIndefiniteToast();
				global::toast::delay = 3000;
				global::toast::modalToast = true;
				emit showToast("No results found");
				keyboardWidget->clearLineEdit();
				global::keyboard::keyboardText = "";
				QFile::remove("/inkbox/gutenberg-search/search_done");
				QTimer::singleShot(3000, this, SLOT(close()));
				break;
			}
		}
		QThread::msleep(500);
	}
}

void GeneralDialog::increaseSize() {
	log("Resizing", className);

	ui->topStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	ui->mainStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	ui->stackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
	int wx = screenGeometry.width();

	int x = wx - 25;
	int y = this->height() * yIncrease;
	this->setFixedWidth(x);
	this->setFixedHeight(y);

	ui->bodyLabel->sizePolicy().setVerticalPolicy(QSizePolicy::Expanding);
	ui->bodyLabel->sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);

	adjust_size();
}
