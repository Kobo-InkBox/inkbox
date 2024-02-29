#include <QDesktopWidget>
#include <QMessageBox>
#include <QScreen>
#include <QScrollBar>
#include <QStringListModel>
#include <QTimer>

#include "device.h"
#include "functions.h"
#include "mainwindow.h"
#include "network.h"
#include "ui.h"
#include "ui_wifidialog.h"
#include "wifidialog.h"
#include "wifilogger.h"

WifiDialog::WifiDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::WifiDialog) {
	Device *device = Device::getSingleton();
	ui->setupUi(this);
	this->setFont(QFont("u001"));

	// Stylesheet, style & misc.
	UI::applyStyle(*this, "eink");

	this->setModal(true);

	ui->wifiCheckBox->setFont(QFont("Inter"));
	ui->returnBtn->setFont(QFont("Inter"));
	ui->returnBtn->setStyleSheet("font-weight: bold");

	ui->refreshBtn->setProperty("type", "borderless");
	ui->stopBtn->setProperty("type", "borderless");
	ui->logBtn->setProperty("type", "borderless");
	ui->returnBtn->setProperty("type", "borderless");

	ui->refreshBtn->setIcon(QIcon(":/resources/refresh.png"));
	ui->stopBtn->setIcon(QIcon(":/resources/stop.png"));
	ui->logBtn->setIcon(QIcon(":/resources/log.png"));

	// Size
	QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
	this->setFixedWidth(screenGeometry.width() / 1.1);

	int halfOfHalfHeight = ((screenGeometry.height() / 2) / 2) / 2;
	int finalHeight = screenGeometry.height() - halfOfHalfHeight * 2;

	this->setFixedHeight(finalHeight);

	// Centering dialog
	int x = (screenGeometry.width() - this->width()) / 2;
	int y = (screenGeometry.height() - this->height()) / 2;
	this->move(x, y);

	// Button sizes
	if (device->getScreenSize() == Device::ScreenSize::SMALL) {
		ui->refreshBtn->setStyleSheet("padding: 20px;");
		ui->stopBtn->setStyleSheet("padding: 20px;");
		ui->logBtn->setStyleSheet("padding: 20px;");
	} else {
		ui->refreshBtn->setStyleSheet("padding: 25px;");
		ui->stopBtn->setStyleSheet("padding: 25px;");
		ui->logBtn->setStyleSheet("padding: 25px;");
	}

	// Set Wi-Fi checkbox state. Ignore the first call.
	global::wifi::wifiState currentWifiState = checkWifiState();
	if (currentWifiState != global::wifi::wifiState::disabled and currentWifiState != global::wifi::wifiState::unknown) {
		ui->wifiCheckBox->setChecked(true);
		// To be sure nothing breaks
		refreshFromWatcher = true;
		ui->refreshBtn->click();
	} else {
		wifiButtonEnabled = true;
		ui->stopBtn->setStyleSheet(ui->stopBtn->styleSheet() + "background-color: lightGray;");
		ui->stopBtn->setEnabled(false);
	}

	// To avoid confusion with reconnecting
	QTimer::singleShot(0, this, SLOT(watcher()));
}

WifiDialog::~WifiDialog() {
	delete ui;
}

void WifiDialog::on_refreshBtn_clicked() {
	log("Clicked refresh button", className);
	if (checkWifiState() == global::wifi::wifiState::disabled) {
		if (refreshFromWatcher == true) {
			refreshFromWatcher = false;
			emit showToast("To scan, turn Wi-Fi on first");
		}
	} else {
		refreshFromWatcher = false;
		launchRefresh();
	}
}

void WifiDialog::launchRefresh() {
	// Order is important
	if (scanInProgress == false) {
		scanInProgress = true;
		ui->refreshBtn->setStyleSheet(ui->refreshBtn->styleSheet() + "background-color: lightGray;");
		ui->refreshBtn->setEnabled(false);

		elapsedSeconds = 0;
		fullList.remove();
		formattedList.remove();
		writeFile("/opt/ibxd", "list_wifi_networks\n");
		QTimer::singleShot(0, this, SLOT(refreshWait()));
	} else {
		log("Scan is already in progress", className);
	}
}

void WifiDialog::refreshWait() {
	if (fullList.exists() == false and formattedList.exists() == false) {
		if (elapsedSeconds == 6) {
			emit showToast("Failed to get networks list");
			ui->refreshBtn->setStyleSheet(ui->refreshBtn->styleSheet() + "background-color: white;");
			ui->refreshBtn->setEnabled(true);
			scanInProgress = false;
		} else {
			elapsedSeconds = elapsedSeconds + 1;
			QTimer::singleShot(1000, this, SLOT(refreshWait()));
		}
	} else {
		log("Retrieved network list successfully", className);
		refreshNetworksList();
	}
}

void WifiDialog::refreshNetworksList() {
	emit killNetworkWidgets();
	QStringList networkList = readFile("/external_root/run/wifi_list_format").split("%%==SPLIT==%%\n");
	QVector<global::wifi::wifiNetworkData> pureNetworkList;
	for (QString network : networkList) {
		QStringList data = network.split("\n");
		int count = 1;
		global::wifi::wifiNetworkData singleNetwork;
		if (data.count() < 4) {
			log("Data lines count is below 4; skipping", className);
			continue;
		}
		for (QString singleData : data) {
			if (count == 1) {
				singleNetwork.mac = singleData;
				log("MAC is: " + singleData, className);
			}
			if (count == 2) {
				if (singleData.isEmpty() == true) {
					log("Network name is empty", className);
				} else {
					log("Network name is: '" + singleData + "'", className);
				}
				singleNetwork.name = singleData;
			}
			if (count == 3) {
				log("Encryption type is: " + singleData, className);
				singleNetwork.encryption = QVariant(singleData).toBool();
			}
			if (count == 4) {
				log("Signal strength is: " + singleData, className);
				singleNetwork.signal = QVariant(singleData).toInt();
			}
			if (count >= 5) {
				log("Skipping additional items from network information", className);
			}
			count = count + 1;
		}
		// Filter out remaining empty networks
		if (singleNetwork.name.isEmpty() == false) {
			pureNetworkList.append(singleNetwork);
		}
	}
	log("Found " + QString::number(pureNetworkList.count()) + " valid networks", className);
	if (pureNetworkList.count() == 0) {
		if (secondScanTry == false) {
			secondScanTry = true;
			if (checkWifiState() != global::wifi::wifiState::disabled) {
				scanInProgress = false;
				QTimer::singleShot(0, this, SLOT(launchRefresh()));
				log("No networks found. Trying one more time");
				return void();
			} else {
				return void();
			}
		} else {
			secondScanTry = false;
			log("No networks found, skipping", className);
			showToastSlot("No networks found");
			ui->refreshBtn->setEnabled(true);
			ui->refreshBtn->setStyleSheet(ui->refreshBtn->styleSheet() + "background-color: white;");
			scanInProgress = false;
			return void();
		}
	}
	QFile currentWifiNameFile = QFile("/external_root/run/current_wifi_name");
	currentWifiNameFile.remove();
	writeFile("/opt/ibxd", "get_current_wifi_name\n");
	usleep(300000); // 0.3s
	// Here, it's looking for the currently connected network to put it on top of the list
	QString currentNetwork = "";
	if (currentWifiNameFile.exists() == true) {
		QString currentWifiNetwork = readFile(currentWifiNameFile.fileName());
		currentWifiNetwork = currentWifiNetwork.replace("\n", "");
		log("Current network name is: " + currentWifiNetwork, className);
		int countVec = 0;
		int vectorNetworkLocation = 9999;
		for (global::wifi::wifiNetworkData wifiNetwork : pureNetworkList) {
			if (currentWifiNetwork.isEmpty() == false and wifiNetwork.name == currentWifiNetwork) {
				log("Found current network in vector", className);
				vectorNetworkLocation = countVec;
				currentNetwork = wifiNetwork.name;
				NetworkWidget *connectedNetwork = new NetworkWidget;
				connectedNetwork->mainData = wifiNetwork;
				// To be really sure that the the info is put there
				connectedNetwork->currentlyConnectedNetwork = currentNetwork;
				connectedNetworkDataParent = wifiNetwork;
				connectedNetworkDataParentSet = true;

				// This doesn't work, so a layout is needed
				// ui->scrollArea->addScrollBarWidget(connectedNetwork, Qt::AlignTop);
				connectedNetwork->applyVariables();
				connect(this, &WifiDialog::killNetworkWidgets, connectedNetwork, &NetworkWidget::closeWrapper);
				connect(connectedNetwork, &NetworkWidget::showToastSignal, this, &WifiDialog::showToastSlot);
				connect(connectedNetwork, &NetworkWidget::refreshScreenSignal, this, &WifiDialog::refreshScreenSlot);
				ui->scrollBarLayout->addWidget(connectedNetwork, Qt::AlignTop);
			} else {
				countVec = countVec + 1;
			}
		}
		if (vectorNetworkLocation != 9999) {
			log("pureNetworkList size is: " + QString::number(pureNetworkList.count()) + ", entry at " + QString::number(vectorNetworkLocation) + " slated for removal", className);
			pureNetworkList.removeAt(vectorNetworkLocation);
		}
	}

	if (pureNetworkList.count() >= 1) {
		// Sort based on signal strength
		QVector<global::wifi::wifiNetworkData> sortedPureNetworkList;
		sortedPureNetworkList.append(pureNetworkList.first());
		pureNetworkList.removeFirst();
		// Possible fix for a segmentation fault
		if (pureNetworkList.isEmpty() == false) {
			for (global::wifi::wifiNetworkData wifiNetwork : pureNetworkList) {
				bool stopIterating = false;
				int counter = 0;
				for (global::wifi::wifiNetworkData wifiNetworkToSort : sortedPureNetworkList) {
					if (stopIterating == false) {
						if (wifiNetwork.signal >= wifiNetworkToSort.signal) {
							sortedPureNetworkList.insert(counter, wifiNetwork);
							stopIterating = true;
						}
						counter = counter + 1;
					}
				}
				// This happens if it's the smallest value, so insert it at the end
				if (stopIterating == false) {
					sortedPureNetworkList.append(wifiNetwork);
				}
			}
		}
		log("There are " + QString::number(sortedPureNetworkList.count()) + " sorted networks", className);

		for (global::wifi::wifiNetworkData wifiNetwork : sortedPureNetworkList) {
			log("Signal strength with sorting: " + QString::number(wifiNetwork.signal), className);
		}

		// And now, handle the remainder of the networks
		for (global::wifi::wifiNetworkData wifiNetwork : sortedPureNetworkList) {
			NetworkWidget *connectedNetwork = new NetworkWidget;
			connectedNetwork->mainData = wifiNetwork;
			connectedNetwork->currentlyConnectedNetwork = currentNetwork;
			connectedNetwork->applyVariables();
			connect(this, &WifiDialog::killNetworkWidgets, connectedNetwork, &NetworkWidget::closeWrapper);
			connect(connectedNetwork, &NetworkWidget::showToastSignal, this, &WifiDialog::showToastSlot);
			connect(connectedNetwork, &NetworkWidget::refreshScreenSignal, this, &WifiDialog::refreshScreenSlot);
			ui->scrollBarLayout->addWidget(connectedNetwork, Qt::AlignTop);
		}
	}
	scannedAtLeastOnce = true;
	ui->refreshBtn->setEnabled(true);
	ui->refreshBtn->setStyleSheet(ui->refreshBtn->styleSheet() + "background-color: white;");
	scanInProgress = false;
	secondScanTry = false;
}

void WifiDialog::on_wifiCheckBox_stateChanged(int arg1) {
	if (ignoreCheckBoxCall == false) {
		connectedNetworkDataParentSet = false;
		log("Wi-Fi dialog checkbox clicked: " + QString::number(arg1), className);
		if (wifiButtonEnabled == true) {
			if (arg1 == 2) {
				log("Turning Wi-Fi on", className);
				// The watcher will scan Wi-Fi
				QTimer::singleShot(0, this, SLOT(turnOnWifi()));
				ui->stopBtn->setStyleSheet(ui->stopBtn->styleSheet() + "background-color: white;");
				ui->stopBtn->setEnabled(true);
			} else {
				log("Turning Wi-Fi off", className);
				QTimer::singleShot(0, this, SLOT(turnOffWifi()));
				// To inform the Wi-Fi icon updater to not show the connected/failed to connect message
				writeFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/stopped", "true");
				ui->stopBtn->setStyleSheet(ui->stopBtn->styleSheet() + "background-color: lightGray;");
				ui->stopBtn->setEnabled(false);
			}
			emit killNetworkWidgets();
		}
		if (wifiButtonEnabled == false) {
			wifiButtonEnabled = true;
		}
	} else {
		ignoreCheckBoxCall = false;
		if (checkWifiState() != global::wifi::wifiState::disabled) {
			emit killNetworkWidgets();
			forceRefresh = true;
		} else {
			emit killNetworkWidgets();
		}
	}
}

void WifiDialog::turnOnWifi() {
	writeFile("/opt/ibxd", "toggle_wifi_on\n");
	// No one will notice this freeze :>
	waitToScan();
}

void WifiDialog::turnOffWifi() {
	writeFile("/opt/ibxd", "toggle_wifi_off\n");
}

void WifiDialog::on_logBtn_clicked() {
	// To avoid half of the information
	if (scannedAtLeastOnce == false and checkWifiState() == global::wifi::wifiState::configured) {
		log("Scanning at least once is needed");
		emit showToast("Scan at least once");
	} else {
		WifiLoggerDialog *wifiLoggerDialog = new WifiLoggerDialog;
		if (connectedNetworkDataParentSet == true) {
			wifiLoggerDialog->connectedNetworkData = connectedNetworkDataParent;
			wifiLoggerDialog->isThereData = true;
		}
		wifiLoggerDialog->exec();
	}
}

void WifiDialog::showToastSlot(QString message) {
	emit showToast(message);
}

void WifiDialog::refreshScreenSlot() {
	this->repaint();
}

/*
	Some documentation used by the watcher
	* connection_manager.sh - Manages all things, launches other processes
	* connect_to_network.sh - All-in-one connection manager. Manages everything, used by IPD, should be used for Wi-Fi reconnections after sleeping/booting
	* get_dhcp.sh - Gets dhcp addresses
	* prepare_changing_wifi.sh - Kills everything, prepares to changing network
	* timesync.sh - Syncs time
	* toggle.sh - Turns Wi-Fi adapter on/off
	* list_networks - Lists networks
	* check_wifi_passphrase.sh - Checks Wi-Fi network passphrase
	* watcher() first watches at processes that could kill other ones
*/

void WifiDialog::watcher() {
	bool killing = checkProcessName("toggle.sh");
	bool changing = checkProcessName("prepare_changing_wifi.sh");
	if (killing == true) {
		setStatusText("Changing Wi-Fi adapter status");
		isToggleRunning = true;
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	if (changing == true) {
		setStatusText("Disconnecting or cleaning up");
		log("prepare_changing_wifi.sh is active", className);
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool reconnection = checkProcessName("connect_to_network.sh");
	if (reconnection == true) {
		forceRefresh = true;
		QFile recName = QFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/essid");
		if (recName.exists() == true) {
			setStatusText("Reconnecting to '" + readFile(recName.fileName()).replace("\n", "") + "'");
		} else {
			// Shouldn't be possible
			setStatusText("Reconnecting after sleep");
		}
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool listing = checkProcessName("list_networks");
	if (listing == true) {
		setStatusText("Scanning available networks");
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool dhcp = checkProcessName("get_dhcp.sh");
	if (dhcp == true) {
		forceRefresh = true;
		setStatusText("Getting IP address");
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool passphraseCheck = checkProcessName("check_wifi_passphrase.sh");
	if (passphraseCheck == true) {
		forceRefresh = true;
		setStatusText("Checking Wi-Fi network passphrase");
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool time = checkProcessName("timesync.sh");
	if (time == true) {
		forceRefresh = true;
		setStatusText("Syncing time");
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	bool connecting = checkProcessName("connection_manager.sh");
	if (connecting == true) {
		forceRefresh = true;
		setStatusText("Connecting to Wi-Fi network");
		QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
		return void();
	}

	if (ui->statusLabel->text() != "Idle") {
		setStatusText("Idle");
	}

	if (unlockCheckBox == true) {
		ui->wifiCheckBox->setEnabled(true);
		unlockCheckBox = false;
	}

	if (isToggleRunning == true) {
		isToggleRunning = false;
		// Make sure the checkbox is in the right state
		if (checkWifiState() == global::wifi::wifiState::disabled) {
			// In this state, ignore forceRefresh to avoid message
			forceRefresh = false;
			if (ui->wifiCheckBox->isChecked() == true) {
				ignoreCheckBoxCall = true;
				ui->wifiCheckBox->setChecked(false);
			}
		} else {
			if (ui->wifiCheckBox->isChecked() == false) {
				ignoreCheckBoxCall = true;
				ui->wifiCheckBox->setChecked(true);
			}
		}
	}

	if (forceRefresh == true) {
		forceRefresh = false;
		refreshFromWatcher = true;
		QTimer::singleShot(1500, this, SLOT(waitToScan()));
	}

	QTimer::singleShot(relaunchMs, this, SLOT(watcher()));
}

void WifiDialog::setStatusText(QString message) {
	ui->statusLabel->setText(message);
}

void WifiDialog::on_stopBtn_clicked() {
	log("Stop button was clicked", className);
	connectedNetworkDataParentSet = false;
	ui->wifiCheckBox->setEnabled(false);
	unlockCheckBox = true;

	// To inform the wifi icon GUI to don't show the connected/failed to connect message
	writeFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/stopped", "true");
	writeFile("/opt/ibxd", "stop_wifi_operations\n");

	QFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/essid").remove();
	QFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/passphrase").remove();

	// This variable just avoids showing the toast, so it is usable here too
	waitToScan();
}

void WifiDialog::on_returnBtn_clicked() {
	this->deleteLater();
	this->close();
}

void WifiDialog::waitToScan() {
	if (checkWifiState() != global::wifi::wifiState::disabled) {
		ui->refreshBtn->click();
	} else {
		QTimer::singleShot(750, this, SLOT(waitToScan()));
	}
}
