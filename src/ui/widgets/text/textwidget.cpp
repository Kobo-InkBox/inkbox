#include "textwidget.h"
#include "functions.h"
#include "ui.h"
#include "ui_textwidget.h"

#include <QFile>
#include <QProcess>

textwidget::textwidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::textwidget) {
	ui->setupUi(this);
	ui->textBrowser->setFont(QFont("u001"));

	// Stylesheet
	UI::applyStyle(*this, "eink");

	if (global::text::textBrowserDialog == true) {
		ui->textBrowser->setStyleSheet("font-size: 9pt");
	}
	ui->textBrowser->setText(global::text::textBrowserContents);
}

textwidget::~textwidget() {
	delete ui;
}
