#include "calendarapp.h"
#include "ui.h"
#include "ui_calendarapp.h"

#include <QFile>

CalendarAppWidget::CalendarAppWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::CalendarAppWidget) {
	ui->setupUi(this);
	CalendarAppWidget::setFont(QFont("u001"));
	ui->backBtn->setFont(QFont("Inter"));
	ui->backBtn->setStyleSheet("font-weight: bold");

	// Stylesheet
	UI::applyStyle(*this, "eink");

	ui->backBtn->setProperty("type", "borderless");
}

CalendarAppWidget::~CalendarAppWidget() {
	delete ui;
}

void CalendarAppWidget::on_backBtn_clicked() {
	CalendarAppWidget::close();
}
