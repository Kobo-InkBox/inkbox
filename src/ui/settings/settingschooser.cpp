#include "settingschooser.h"
#include "device.h"
#include "functions.h"
#include "powerdaemonsettings.h"
#include "ui.h"
#include "ui_settingschooser.h"

#include <QFile>
#include <QScreen>

SettingsChooserWidget::SettingsChooserWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::SettingsChooserWidget) {
	ui->setupUi(this);

	// Stylesheet
	UI::applyStyle(*this, "eink");

	// UI tweaks
	if (checkconfig("/opt/inkbox_kobox_support") == false) {
		ui->koboxSettingsBtn->hide();
		ui->line_3->hide();
	}

	// Getting the screen's size
	sW = QGuiApplication::screens()[0]->size().width();
	sH = QGuiApplication::screens()[0]->size().height();

	Device::ScreenSize screenSize = Device::getSingleton()->getScreenSize();
	QString padding = "";
	if (screenSize == Device::ScreenSize::SMALL) {
		homeIconWidth = sW / 18;
		homeIconHeight = sW / 18;
		padding = "18px";
	} else if (screenSize == Device::ScreenSize::MEDIUM) {
		homeIconWidth = sW / 20;
		homeIconHeight = sW / 20;
		padding = "20px";
	} else if (screenSize == Device::ScreenSize::LARGE) {
		homeIconWidth = sW / 20;
		homeIconHeight = sW / 20;
		padding = "25px";
	} else if (screenSize == Device::ScreenSize::VERY_LARGE) {
		homeIconWidth = sW / 20;
		homeIconHeight = sW / 20;
		padding = "40px";
	}
	ui->inkboxSettingsBtn->setStyleSheet("padding: " + padding + "; Text-align: left");
	ui->koboxSettingsBtn->setStyleSheet("padding: " + padding + "; Text-align: left");
	ui->powerSettingsBtn->setStyleSheet("padding: " + padding + "; Text-align: left");

	ui->inkboxSettingsBtn->setProperty("type", "borderless");
	ui->koboxSettingsBtn->setProperty("type", "borderless");
	ui->powerSettingsBtn->setProperty("type", "borderless");

	ui->inkboxSettingsBtn->setText("\t\t\tInkBox settings");
	ui->koboxSettingsBtn->setText("\t\t\tKoBox settings");
	ui->powerSettingsBtn->setText("\t\t\tPower settings");

	ui->inkboxSettingsBtn->setIcon(QIcon(":/resources/settings.png"));
	ui->inkboxSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));
	ui->koboxSettingsBtn->setIcon(QIcon(":/resources/X11.png"));
	ui->koboxSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));
	ui->powerSettingsBtn->setIcon(QIcon(":/resources/power.png"));
	ui->powerSettingsBtn->setIconSize(QSize(homeIconWidth, homeIconHeight));
}

SettingsChooserWidget::~SettingsChooserWidget() {
	delete ui;
}

void SettingsChooserWidget::on_inkboxSettingsBtn_clicked() {
	log("Launching InkBox Settings", className);
	settingsWindow = new SettingsWidget();
	settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
	connect(settingsWindow, SIGNAL(showToast(QString)), SLOT(showToastNative(QString)));
	connect(settingsWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToastNative()));
	settingsWindow->showFullScreen();
}

void SettingsChooserWidget::on_koboxSettingsBtn_clicked() {
	log("Launching KoBox Settings", className);
	koboxSettingsWindow = new KoboxSettingsWidget();
	koboxSettingsWindow->setAttribute(Qt::WA_DeleteOnClose);
	koboxSettingsWindow->showFullScreen();
}

void SettingsChooserWidget::on_powerSettingsBtn_clicked() {
	log("Launching Power Daemon Settings", className);
	PowerDaemonSettingsWidget *powerSettingsWindow = new PowerDaemonSettingsWidget();
	powerSettingsWindow->setAttribute(Qt::WA_DeleteOnClose);
	powerSettingsWindow->showFullScreen();
}

void SettingsChooserWidget::showToastNative(QString messageToDisplay) {
	emit showToast(messageToDisplay);
}

void SettingsChooserWidget::closeIndefiniteToastNative() {
	emit closeIndefiniteToast();
}
