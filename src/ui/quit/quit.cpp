#include "quit.h"
#include "functions.h"
#include "ui.h"
#include "ui_quit.h"
#include <linux/input.h>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QProcess>
#include <QScreen>

QuitWidget::QuitWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::QuitWidget) {
	ui->setupUi(this);
	ui->backBtn->setFont(QFont("u001"));

	ui->pushButton->setProperty("type", "borderless");
	ui->pushButton_2->setProperty("type", "borderless");
	ui->pushButton_3->setProperty("type", "borderless");
	ui->pushButton_4->setProperty("type", "borderless");
	ui->backBtn->setProperty("type", "borderless");

	// Stylesheet
	UI::applyStyle(*this, "eink");

	// Getting the screen's size
	float sW = QGuiApplication::screens()[0]->size().width();
	float sH = QGuiApplication::screens()[0]->size().height();
	// Defining what the "Quit" icon size will be
	float stdIconWidth = sW / 1.25;
	float stdIconHeight = sH / 1.25;

	QPixmap pixmap(":/resources/exit.png");
	QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	ui->label->setPixmap(scaledPixmap);

	if (checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/9-deepSleep") == true) {
		ui->pushButton_3->setText("Deep suspend");
	}
}

QuitWidget::~QuitWidget() {
	delete ui;
}

void QuitWidget::on_pushButton_clicked() {
	// Prevent strange "appearing from nowhere" low battery or critical battery alerts due to sysfs unmount
	global::battery::showCriticalBatteryAlert = false;
	global::battery::showLowBatteryDialog = false;

	// GUI apps
	updateUserAppsMainJsonFile();

	poweroff(true);
	qApp->quit();
}

void QuitWidget::on_pushButton_2_clicked() {
	global::battery::showCriticalBatteryAlert = false;
	global::battery::showLowBatteryDialog = false;

	// GUI apps
	updateUserAppsMainJsonFile();

	reboot(true);
	qApp->quit();
}

void QuitWidget::on_pushButton_4_clicked() {
	log("Restarting InkBox", className);
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void QuitWidget::on_backBtn_clicked() {
	QuitWidget::close();
}

void QuitWidget::on_pushButton_3_clicked() {
	log("Suspending", className);
	if (checkconfig("/mnt/onboard/.adds/inkbox/.config/20-sleep_daemon/9-deepSleep") == true) {
		writeFile("/dev/ipd/sleepCall", "deepSleep");
	} else {
		writeFile("/dev/ipd/sleepCall", "sleep");
	}
}
