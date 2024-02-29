#include "sleepdialog.h"
#include "ui.h"
#include "ui_sleepdialog.h"

#include "functions.h"

SleepDialog::SleepDialog(QDialog *parent) :
		QDialog(parent),
		ui(new Ui::SleepDialog) {
	ui->setupUi(this);

	// Stylesheet
	UI::applyStyle(*this, "eink");
}

SleepDialog::~SleepDialog() {
	delete ui;
}

void SleepDialog::launchSleepDialog() {
	log("Launching sleep dialog", className);
	this->exec();
}

void SleepDialog::hideSleepDialog() {
	log("Hiding sleep dialog", className);
	this->hide();
}
