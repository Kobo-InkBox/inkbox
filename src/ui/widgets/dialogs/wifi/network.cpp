#include "network.h"
#include "connectiondialog.h"
#include "device.h"
#include "ui.h"
#include "ui_network.h"

#include <QScreen>

NetworkWidget::NetworkWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::NetworkWidget) {
	ui->setupUi(this);
	this->setFont(QFont("u001"));

	// Stylesheet, style & misc.
	UI::applyStyle(*this, "eink");

	// Buttons
	ui->encryptionIcon->setProperty("type", "borderless");
	ui->encryptionIcon->setStyleSheet("QPushButton[type='borderless']:pressed { background: white; color: white; border: none; }");
	ui->signalStrengthIcon->setProperty("type", "borderless");
	ui->signalStrengthIcon->setStyleSheet("QPushButton[type='borderless']:pressed { background: white; color: white; border: none; }");
	ui->enterButton->setProperty("type", "borderless");
}

NetworkWidget::~NetworkWidget() {
	delete ui;
}

void NetworkWidget::applyVariables() {
	log("Applying variables for network", className);

	QString percent = "%";
	if (mainData.signal < 100) {
		percent.append(" ");
	}
	ui->signalStrengthLabel->setText(QString::number(mainData.signal) + percent);

	// Limit name size
	int truncateThreshold;
	if (Device::getSingleton()->getScreenSize() == Device::SMALL) {
		truncateThreshold = 12;
	} else {
		truncateThreshold = 20;
	}
	int nameLength = mainData.name.length();
	QString name = mainData.name;
	if (nameLength > truncateThreshold) {
		name.chop(nameLength - truncateThreshold);
		name.append("...");
	}
	ui->nameLabel->setText(name);

	if (mainData.encryption == true) {
		ui->encryptionIcon->setIcon(QIcon(":/resources/lock.png"));
	} else {
		ui->encryptionIcon->setIcon(QIcon(":/resources/public.png"));
	}

	if (currentlyConnectedNetwork == mainData.name) {
		ui->frame->setStyleSheet(".QFrame{background-color: lightGray; border: 2px solid black; border-radius: 10px;}");
		ui->signalStrengthLabel->setStyleSheet("background-color: lightGray;");
		ui->nameLabel->setStyleSheet("background-color: lightGray;");
		ui->encryptionIcon->setStyleSheet("background-color: lightGray;");
		ui->enterButton->setStyleSheet("background-color: lightGray;");

		ui->signalStrengthIcon->setStyleSheet("QPushButton {background-color: lightGray; border:  none}; QPushButton[type='borderless']:pressed { background: lightGray; color: lightGray; border: none; }");
		ui->encryptionIcon->setStyleSheet("QPushButton {background-color: lightGray; border:  none}; QPushButton[type='borderless']:pressed { background: lightGray; color: lightGray; border: none; }");
		ui->enterButton->setStyleSheet("QPushButton {background-color: lightGray; border:  none}; QPushButton[type='borderless']:pressed { background: lightGray; color: lightGray; border: none; }");
	} else {
		ui->frame->setStyleSheet(".QFrame{background-color: white; border: 2px solid black; border-radius: 10px;}");
	}

	if (mainData.signal >= 0 and mainData.signal <= 25) {
		ui->signalStrengthIcon->setIcon(QIcon(":/resources/wifi-0.png"));
	} else if (mainData.signal >= 25 and mainData.signal < 50) {
		ui->signalStrengthIcon->setIcon(QIcon(":/resources/wifi-25.png"));
	} else if (mainData.signal >= 50 and mainData.signal < 75) {
		ui->signalStrengthIcon->setIcon(QIcon(":/resources/wifi-50.png"));
	} else if (mainData.signal >= 75 and mainData.signal < 100) {
		ui->signalStrengthIcon->setIcon(QIcon(":/resources/wifi-75.png"));
	} else if (mainData.signal == 100) {
		ui->signalStrengthIcon->setIcon(QIcon(":/resources/wifi-100.png"));
	}
}

void NetworkWidget::on_enterButton_clicked() {
	ConnectionDialog *newConnectionDialog = new ConnectionDialog;
	newConnectionDialog->connectedNetworkData = mainData;
	newConnectionDialog->currentlyConnectedNetworkName = currentlyConnectedNetwork;
	newConnectionDialog->applyVariables();
	connect(newConnectionDialog, &ConnectionDialog::showToastSignal, this, &NetworkWidget::showToastSlot);
	connect(newConnectionDialog, &ConnectionDialog::refreshScreenSignal, this, &NetworkWidget::refreshScreenSlot);
	newConnectionDialog->exec();
}

void NetworkWidget::closeWrapper() {
	this->deleteLater();
	this->close();
}

void NetworkWidget::showToastSlot(QString message) {
	emit showToastSignal(message);
}

void NetworkWidget::refreshScreenSlot() {
	emit refreshScreenSignal();
}
