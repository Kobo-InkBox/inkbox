#include "encryptionmanager.h"
#include "ui.h"
#include "ui_encryptionmanager.h"

#include "functions.h"
#include "usbmsSplash.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QScreen>
#include <QTimer>

CryptoManagerWidget::CryptoManagerWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::CryptoManagerWidget) {
	ui->setupUi(this);
	ui->descriptionLabel->setFont(QFont("u001"));
	ui->successDescriptionLabel->setFont(QFont("u001"));
	ui->failureDescriptionLabel->setFont(QFont("u001"));
	ui->warningDescriptionLabel->setFont(QFont("u001"));

	// Stylesheet
	UI::applyStyle(*this, "eink");

	ui->encryptionSetupLabel->setStyleSheet("font-size: 15pt; font-weight: bold");
	ui->descriptionLabel->setStyleSheet("font-size: 10pt");
	ui->successLabel->setStyleSheet("font-size: 15pt; font-weight: bold");
	ui->successDescriptionLabel->setStyleSheet("font-size: 10pt");
	ui->failureLabel->setStyleSheet("font-size: 15pt; font-weight: bold");
	ui->failureDescriptionLabel->setStyleSheet("font-size: 10pt");
	ui->setupContinueBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->setupAbortBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->exitSuccessBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->failureContinueBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->acceptBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->usbmsBtn->setStyleSheet("background: lightGrey; border: 3px solid black; color: black; padding: 10px; outline: none; font-size: 10pt; font-weight: bold; border-radius: 10px");
	ui->warningLabel->setStyleSheet("font-size: 15pt; font-weight: bold");
	ui->warningDescriptionLabel->setStyleSheet("font-size: 10pt");

	// Getting the screen's size
	float sW = QGuiApplication::screens()[0]->size().width();
	float sH = QGuiApplication::screens()[0]->size().height();
	float stdIconWidth;
	float stdIconHeight;
	{
		stdIconWidth = sW / 1.50;
		stdIconHeight = sH / 1.50;
		QPixmap pixmap(":/resources/encryption.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->encryptionImageLabel->setPixmap(scaledPixmap);
	}
	{
		stdIconWidth = sW / 1.65;
		stdIconHeight = sH / 1.65;
		QPixmap pixmap(":/resources/check-display.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->checkImageLabel->setPixmap(scaledPixmap);
	}
	{
		stdIconWidth = sW / 1.65;
		stdIconHeight = sH / 1.65;
		QPixmap pixmap(":/resources/error.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->failureImageLabel->setPixmap(scaledPixmap);
	}
	{
		stdIconWidth = sW / 1.50;
		stdIconHeight = sH / 1.50;
		QPixmap pixmap(":/resources/alert-triangle.png");
		QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->warningImageLabel->setPixmap(scaledPixmap);
	}

	hourglassAnimationWidgetWindow = new hourglassAnimationWidget();
	ui->hourglassWidget->insertWidget(0, hourglassAnimationWidgetWindow);
	ui->hourglassWidget->setCurrentIndex(0);

	setDefaultWorkDir();
	if (checkconfig(".config/18-encrypted_storage/initial_setup_done") == true or checkconfig("/external_root/run/encfs_repack") == true) {
		ui->activityWidget->hide();
		if (checkconfig("/external_root/run/encfs_repack") == false) {
			setupPassphraseDialogMode = 1;
		} else {
			setupPassphraseDialogMode = 2;
		}
		QTimer::singleShot(500, this, SLOT(setupPassphraseDialog()));
	} else {
		QDir dir("/mnt/onboard/onboard/encfs-dropbox");
		if (dir.isEmpty()) {
			ui->activityWidget->setCurrentIndex(4);
			mkEncfsDirs();
		}
	}
}

CryptoManagerWidget::~CryptoManagerWidget() {
	delete ui;
}

void CryptoManagerWidget::on_setupContinueBtn_clicked() {
	setupPassphraseDialogMode = 0;
	setupPassphraseDialog();
}

void CryptoManagerWidget::setupPassphraseDialog() {
	/*
	 * setupPassphraseDialogMode can be:
	 * 0: Initial setup
	 * 1: Normal behavior
	 * 2: Repack
	 */
	log("Showing passphrase dialog for mode " + QString::number(setupPassphraseDialogMode), className);
	ui->activityWidget->hide();
	this->setStyleSheet("background-color: black");
	global::keyboard::keyboardDialog = true;
	global::keyboard::encfsDialog = true;
	global::keyboard::keyboardText = "";
	generalDialogWindow = new GeneralDialog();
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	connect(generalDialogWindow, SIGNAL(refreshScreen()), SLOT(refreshScreen()));
	if (setupPassphraseDialogMode == 0) {
		connect(generalDialogWindow, SIGNAL(destroyed(QObject *)), SLOT(setupEncryptedStorage()));
	} else if (setupPassphraseDialogMode == 1) {
		connect(generalDialogWindow, SIGNAL(destroyed(QObject *)), SLOT(unlockEncryptedStorage()));
	} else {
		connect(generalDialogWindow, SIGNAL(destroyed(QObject *)), SLOT(repackEncryptedStorage()));
	}
	connect(generalDialogWindow, SIGNAL(showToast(QString)), SLOT(showToast(QString)));
	generalDialogWindow->show();
}

void CryptoManagerWidget::on_setupAbortBtn_clicked() {
	log("Aborting setup", className);
	setDefaultWorkDir();
	writeFile(".config/18-encrypted_storage/status", "false");
	quit_restart();
}

void CryptoManagerWidget::quit_restart() {
	log("Restarting InkBox", className);
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void CryptoManagerWidget::refreshScreen() {
	this->repaint();
}

void CryptoManagerWidget::showToast(QString messageToDisplay) {
	global::toast::message = messageToDisplay;
	toastWindow = new toast(this);
	toastWindow->setAttribute(Qt::WA_DeleteOnClose);
	connect(toastWindow, SIGNAL(refreshScreen()), SLOT(refreshScreen()));
	toastWindow->show();
}

void CryptoManagerWidget::setupEncryptedStorage() {
	log("Setting up encrypted storage", className);
	this->setStyleSheet("background-color: white");
	ui->activityWidget->show();
	if (global::encfs::cancelSetup == true) {
		global::encfs::cancelSetup = false;
		ui->activityWidget->setCurrentIndex(0);
	} else {
		mkEncfsDirs();
		QString bootstrapPassphrase = global::encfs::passphrase;
		global::encfs::passphrase = "";
		writeFile("/external_root/run/encfs/encrypted_storage_create", "true");
		writeFile("/external_root/run/encfs/encrypted_storage_bootstrap_files_location", "/data/onboard/encfs-dropbox");
		writeFile("/external_root/run/encfs/encrypted_storage_bootstrap_archive_location", "/data/onboard/data.encfs");
		writeFile("/external_root/run/encfs/encrypted_storage_bootstrap_passphrase", bootstrapPassphrase);
		setDefaultWorkDir();
		writeFile(".config/18-encrypted_storage/storage_list", "/data/onboard/encfs-decrypted");
		writeFile("/opt/ibxd", "encfs_restart\n");
		bool exitStatus;
		ui->activityWidget->setCurrentIndex(3);
		QTimer *t = new QTimer(this);
		t->setInterval(1000);
		connect(t, &QTimer::timeout, [&]() {
			if (QFile::exists("/external_root/run/encrypted_storage_bootstrap_setup")) {
				exitStatus = checkconfig("/external_root/run/encrypted_storage_bootstrap_setup");
				QFile::remove("/external_root/run/encrypted_storage_bootstrap_setup");
				setupExitWidget(exitStatus);
			}
		});
		t->start();
	}
}

void CryptoManagerWidget::unlockEncryptedStorage() {
	log("Trying to unlock encrypted storage", className);
	if (global::encfs::cancelSetup == true) {
		global::encfs::cancelSetup = false;
		poweroff(true);
		qApp->quit();
	} else {
		this->setStyleSheet("background-color: white");
		ui->activityWidget->show();
		mkEncfsDirs();
		QString passphrase = global::encfs::passphrase;
		global::encfs::passphrase = "";
		writeFile("/external_root/run/encfs/encrypted_storage_archive", "/data/onboard/data.encfs");
		writeFile("/external_root/run/encfs/encrypted_storage_mountpoint", "/data/onboard/encfs-decrypted");
		writeFile("/external_root/run/encfs/encrypted_storage_bindmount", "/kobo/mnt/onboard/onboard/encfs-decrypted");
		writeFile("/external_root/run/encfs/encrypted_storage_passphrase", passphrase);
		writeFile("/opt/ibxd", "encfs_restart\n");
		bool exitStatus;

		QString passphraseTriesStr = readFile("/inkbox/encryptedStoragePassphraseTries");
		if (passphraseTriesStr.isEmpty()) {
			passphraseTries = 0;
		} else {
			passphraseTries = passphraseTriesStr.toInt();
			passphraseTries++;
		}

		ui->activityWidget->setCurrentIndex(3);
		QTimer *t = new QTimer(this);
		t->setInterval(1000);
		connect(t, &QTimer::timeout, [&]() {
			if (QFile::exists("/external_root/run/encfs_mounted")) {
				exitStatus = true;
				if (readFile("/external_root/run/encfs_mounted") != "true\n") {
					exitStatus = false;
					QFile::remove("/external_root/run/encfs_mounted");
					QString function = __func__;
					log(function + ": Invalid passphrase", className);
					if (setupMessageBoxRan == false) {
						int delay = 0;
						if (passphraseTries <= 3) {
							if (passphraseTries == 0) {
								writeFile("/inkbox/encryptedStoragePassphraseTries", "0");
								delay = 5000;
							} else if (passphraseTries == 1) {
								writeFile("/inkbox/encryptedStoragePassphraseTries", "1");
								delay = 10000;
							} else if (passphraseTries == 2) {
								writeFile("/inkbox/encryptedStoragePassphraseTries", "2");
								delay = 20000;
							} else if (passphraseTries >= 3) {
								writeFile("/inkbox/encryptedStoragePassphraseTries", "3");
								unsigned long currentEpoch = QDateTime::currentSecsSinceEpoch();
								currentEpoch += 86400;
								QString unlockTime_str = QString::number(currentEpoch);
								global::encfs::unlockTime = QDateTime::fromTime_t(currentEpoch).toString();
								global::encfs::lockdownMessage = "FATAL: 4 invalid passphrase tries, locking down device until " + global::encfs::unlockTime;
								writeFile("/external_root/boot/flags/ENCRYPT_LOCK", unlockTime_str + "\n");
								global::encfs::lockdown = true;
								setupMessageBoxRan = true;

								alertWindow = new SplashAlertWidget();
								alertWindow->setAttribute(Qt::WA_DeleteOnClose);
								alertWindow->showFullScreen();
							}

							if (passphraseTries <= 2) {
								QTimer::singleShot(delay, this, SLOT(setupFailedAuthenticationMessageBox()));
								setupMessageBoxRan = true;
							}
						}
					}
				} else {
					log("Unlocking successful", className);
					quit_restart();
				}
			}
		});
		t->start();
	}
}

void CryptoManagerWidget::mkEncfsDirs() {
	log("Creating encrypted storage directories", className);
	QDir encfsDir;
	QString encfsPath("/external_root/run/encfs");
	encfsDir.mkpath(encfsPath);
	QDir dumpDir;
	QString dumpPath("/mnt/onboard/onboard/encfs-dropbox");
	dumpDir.mkpath(dumpPath);
	QDir decDir;
	QString decPath("/mnt/onboard/onboard/encfs-decrypted");
	decDir.mkpath(decPath);
}

void CryptoManagerWidget::on_exitSuccessBtn_clicked() {
	setDefaultWorkDir();
	writeFile(".config/18-encrypted_storage/initial_setup_done", "true");
	quit_restart();
}

void CryptoManagerWidget::setupExitWidget(bool exitStatus) {
	if (setupExitWidgetRan == false) {
		if (exitStatus == true) {
			ui->activityWidget->setCurrentIndex(1);
		} else {
			writeFile(".config/18-encrypted_storage/status", "false");
			ui->activityWidget->setCurrentIndex(2);
		}
		setupExitWidgetRan = true;
	}
}

void CryptoManagerWidget::on_failureContinueBtn_clicked() {
	setDefaultWorkDir();
	writeFile(".config/18-encrypted_storage/initial_setup_done", "true");
	quit_restart();
}

void CryptoManagerWidget::setupFailedAuthenticationMessageBox() {
	log("Showing 'Authentication failed' message box", className);
	ui->activityWidget->hide();
	QMessageBox::critical(this, tr("Invalid argument"), tr("<font face='u001'>Invalid passphrase. Please try again.</font>"));
	quit_restart();
}

void CryptoManagerWidget::on_acceptBtn_clicked() {
	writeFile(".config/18-encrypted_storage/status", "false");
	quit_restart();
}

void CryptoManagerWidget::on_usbmsBtn_clicked() {
	log("Showing USBMS splash", className);
	global::usbms::launchUsbms = true;
	usbmsWindow = new USBMSSplashWidget();
	usbmsWindow->setAttribute(Qt::WA_DeleteOnClose);
	usbmsWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
	usbmsWindow->show();
}

void CryptoManagerWidget::repackEncryptedStorage() {
	log("Repacking encrypted storage", className);
	if (global::encfs::cancelSetup == true) {
		global::encfs::cancelSetup = false;
		quit_restart();
	} else {
		this->setStyleSheet("background-color: white");
		ui->activityWidget->show();
		mkEncfsDirs();
		QString passphrase = global::encfs::passphrase;
		global::encfs::passphrase = "";
		writeFile("/external_root/run/encfs/encrypted_storage_repack_passphrase", passphrase);
		QFile::remove("/external_root/run/openrc/started/encfs");
		writeFile("/opt/ibxd", "encfs_restart\n");
		bool exitStatus;
		ui->activityWidget->setCurrentIndex(3);
		QTimer *t = new QTimer(this);
		t->setInterval(1000);
		connect(t, &QTimer::timeout, [&]() {
			if (QFile::exists("/external_root/run/encfs_repack_status")) {
				exitStatus = checkconfig("/external_root/run/encfs_repack_status");
				QFile::remove("/external_root/run/encfs_repack_status");
				setupExitWidget(exitStatus);
			}
		});
		t->start();
	}
}
