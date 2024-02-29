#include "brightnessdialog.h"
#include "device.h"
#include "functions.h"
#include "ui.h"
#include "ui_brightnessdialog.h"

#include <QDebug>
#include <QFile>
#include <QFont>
#include <QFontDatabase>

BrightnessDialog::BrightnessDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::BrightnessDialog) {
	ui->setupUi(this);
	BrightnessDialog::setFont(QFont("u001"));

	// Stylesheet, style & misc.
	UI::applyStyle(*this, "eink");

	ui->quitBtn->setProperty("type", "borderless");
	ui->quitBtn->setText("");
	ui->quitBtn->setIcon(QIcon(":/resources/close.png"));
	ui->okBtn->setProperty("type", "borderless");
	ui->okBtn->setText("");
	ui->okBtn->setIcon(QIcon(":/resources/check.png"));
	ui->decBtn->setProperty("type", "borderless");
	ui->decBtn->setText("");
	ui->decBtn->setIcon(QIcon(":/resources/minus.png"));
	ui->incBtn->setProperty("type", "borderless");
	ui->incBtn->setText("");
	ui->incBtn->setIcon(QIcon(":/resources/plus.png"));
	ui->brightnessLabel->setStyleSheet("padding-left: 125px; padding-right: 125px; font:bold");
	ui->valueLabel->setStyleSheet("font-size: 9pt");
	ui->warmthValueLabel->setStyleSheet("font-size: 9pt");

	int value;
	int warmthValue;

	Device *platform = Device::getSingleton();

	if (platform->supportsWarmLight()) {
		ui->warmthDecBtn->setProperty("type", "borderless");
		ui->warmthIncBtn->setProperty("type", "borderless");
		ui->warmthDecBtn->setText("");
		ui->warmthIncBtn->setText("");
		ui->warmthDecBtn->setIcon(QIcon(":/resources/nightmode-empty.png"));
		ui->warmthIncBtn->setIcon(QIcon(":/resources/nightmode-full.png"));
		ui->warmthSlider->setMaximum(Device::getSingleton()->getMaxWarmLightValue());

		warmthValue = platform->getWarmth();
		ui->warmthSlider->setValue(warmthValue);
		if (warmthValue == 0) {
			ui->warmthValueLabel->setText("0");
		}
	} else {
		ui->warmthSlider->hide();
		ui->warmthDecBtn->hide();
		ui->warmthIncBtn->hide();
		ui->warmthValueLabel->hide();
		ui->warmthSlider->deleteLater();
		ui->warmthDecBtn->deleteLater();
		ui->warmthIncBtn->deleteLater();
		ui->gridLayout_5->deleteLater();
		ui->warmthValueLabel->deleteLater();
		this->adjustSize();
	}

	value = Device::getSingleton()->getBrightness();

	// Setting the slider to the appropriate position
	ui->horizontalSlider->setValue(value);

	// Showing brightness percentage on label
	QString valueStr = QString::number(value);
	valueStr = valueStr.append("%");
	ui->valueLabel->setText(valueStr);

	// Saving current brightness value in case we want to go backwards
	oldValue = value;
	if (Device::getSingleton()->supportsWarmLight()) {
		oldWarmthValue = warmthValue;
	}
}

BrightnessDialog::~BrightnessDialog() {
	delete ui;
}

void BrightnessDialog::on_quitBtn_clicked() {
	Device *platform = Device::getSingleton();
	// Reverting back to the old value
	platform->setBrightness(oldValue);
	if (platform->supportsWarmLight()) {
		platform->setWarmth(oldWarmthValue);
	}

	// Leaving
	BrightnessDialog::close();
}

void BrightnessDialog::on_horizontalSlider_valueChanged(int value) {
	Device::getSingleton()->setBrightness(value);
	QString valueStr = QString::number(value);
	valueStr = valueStr.append("%");
	ui->valueLabel->setText(valueStr);
}

void BrightnessDialog::on_incBtn_clicked() {
	int value = ui->horizontalSlider->value();
	value = value + 1;
	ui->horizontalSlider->setValue(value);
}

void BrightnessDialog::on_decBtn_clicked() {
	int value = ui->horizontalSlider->value();
	value = value - 1;
	ui->horizontalSlider->setValue(value);
}

void BrightnessDialog::on_okBtn_clicked() {
	Device *device = Device::getSingleton();

	// Get set brightness value
	int brightnessValue = ui->horizontalSlider->value();
	int warmthValue;

	// Write brightness config
	log("Display brightness set to " + QString::number(brightnessValue), className);
	device->setBrightness(brightnessValue);
	if (device->supportsWarmLight()) {
		warmthValue = ui->warmthSlider->value();
		log("Display warmth set to " + QString::number(warmthValue), className);
		device->setWarmth(warmthValue);
	}

	// Leaving
	BrightnessDialog::close();
}

void BrightnessDialog::on_warmthSlider_valueChanged(int value) {
	Device::getSingleton()->setWarmth(value);
	QString valueStr = QString::number(value);
	ui->warmthValueLabel->setText(valueStr);
}

void BrightnessDialog::on_warmthDecBtn_clicked() {
	int value = ui->warmthSlider->value();
	value = value - 1;
	ui->warmthSlider->setValue(value);
}

void BrightnessDialog::on_warmthIncBtn_clicked() {
	int value = ui->warmthSlider->value();
	value = value + 1;
	ui->warmthSlider->setValue(value);
}
