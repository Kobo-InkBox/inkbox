#include "reader.h"
#include "device.h"
#include "functions.h"
#include "ui.h"
#include "ui_reader.h"

#include <QDesktopWidget>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsScene>
#include <QIcon>
#include <QJsonDocument>
#include <QMessageBox>
#include <QProcess>
#include <QScreen>
#include <QSize>
#include <QTextCursor>
#include <QTime>
#include <QTimer>

using namespace std;

ReaderWidget::ReaderWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::ReaderWidget) {
	Device *device = Device::getSingleton();

	// Elements
	graphicsScene = new QGraphicsScene(this);

	// Variables
	global::battery::showLowBatteryDialog = true;
	global::battery::showCriticalBatteryAlert = true;
	global::reader::highlightAlreadyDone = false;
	if (global::reader::bookIsEpub == true) {
		is_epub = true;
	}
	mupdf::convertRelativeValues = false;
	wordwidgetLock = false;
	textDialogLock = false;
	goToSavedPageDone = false;
	initialPdfRotationDone = false;
	global::reader::textAlignment = 0;
	global::reader::lineSpacing = 0;
	global::reader::font = "Source Serif Pro";
	global::reader::fontSize = 10;
	global::reader::margins = 1;
	global::reader::currentViewportText = "";

	ui->setupUi(this);
	ui->fontLabel->setFont(QFont("u001"));
	ui->sizeLabel->setFont(QFont("u001"));
	ui->sizeValueLabel->setFont(QFont("Inter"));
	ui->lineSpacingLabel->setFont(QFont("u001"));
	ui->lineSpacingValueLabel->setFont(QFont("Inter"));
	ui->marginsLabel->setFont(QFont("u001"));
	ui->marginsValueLabel->setFont(QFont("Inter"));
	ui->alignmentLabel->setFont(QFont("u001"));
	ui->fontChooser->setFont(QFont("u001"));
	ui->definitionStatusLabel->setFont(QFont("u001"));
	ui->pageProgressBar->setFont(QFont("u001"));

	ui->previousBtn->setProperty("type", "borderless");
	ui->nextBtn->setProperty("type", "borderless");
	ui->optionsBtn->setProperty("type", "borderless");
	ui->homeBtn->setProperty("type", "borderless");
	ui->brightnessBtn->setProperty("type", "borderless");
	ui->alignLeftBtn->setProperty("type", "borderless");
	ui->alignRightBtn->setProperty("type", "borderless");
	ui->alignCenterBtn->setProperty("type", "borderless");
	ui->alignLeftBtn->setProperty("type", "borderless");
	ui->alignJustifyBtn->setProperty("type", "borderless");
	ui->infoCloseBtn->setProperty("type", "borderless");
	ui->saveWordBtn->setProperty("type", "borderless");
	ui->previousDefinitionBtn->setProperty("type", "borderless");
	ui->nextDefinitionBtn->setProperty("type", "borderless");
	ui->nightModeBtn->setProperty("type", "borderless");
	ui->searchBtn->setProperty("type", "borderless");
	ui->gotoBtn->setProperty("type", "borderless");
	ui->increaseScaleBtn->setProperty("type", "borderless");
	ui->decreaseScaleBtn->setProperty("type", "borderless");
	ui->quitBtn->setProperty("type", "borderless");
	ui->viewHighlightsBtn->setProperty("type", "borderless");

	// Icons
	ui->alignLeftBtn->setText("");
	ui->alignLeftBtn->setIcon(QIcon(":/resources/align-left.png"));
	ui->alignRightBtn->setText("");
	ui->alignRightBtn->setIcon(QIcon(":/resources/align-right.png"));
	ui->alignCenterBtn->setText("");
	ui->alignCenterBtn->setIcon(QIcon(":/resources/align-center.png"));
	ui->alignJustifyBtn->setText("");
	ui->alignJustifyBtn->setIcon(QIcon(":/resources/align-justify.png"));
	ui->infoCloseBtn->setText("");
	ui->infoCloseBtn->setIcon(QIcon(":/resources/close.png"));
	ui->saveWordBtn->setText("");
	ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
	ui->previousDefinitionBtn->setText("");
	ui->previousDefinitionBtn->setIcon(QIcon(":/resources/chevron-left.png"));
	ui->nextDefinitionBtn->setText("");
	ui->nextDefinitionBtn->setIcon(QIcon(":/resources/chevron-right.png"));
	ui->homeBtn->setText("");
	ui->homeBtn->setIcon(QIcon(":/resources/home.png"));
	ui->brightnessBtn->setText("");
	ui->brightnessBtn->setIcon(QIcon(":/resources/frontlight.png"));
	ui->searchBtn->setText("");
	ui->searchBtn->setIcon(QIcon(":/resources/search.png"));
	ui->increaseScaleBtn->setText("");
	ui->increaseScaleBtn->setIcon(QIcon(":/resources/zoom-in.png"));
	ui->decreaseScaleBtn->setText("");
	ui->decreaseScaleBtn->setIcon(QIcon(":/resources/zoom-out.png"));
	ui->quitBtn->setText("");
	ui->quitBtn->setIcon(QIcon(":/resources/power.png"));
	ui->previousBtn->setText("");
	ui->previousBtn->setIcon(QIcon(":/resources/arrow-left.png"));
	ui->optionsBtn->setText("");
	ui->optionsBtn->setIcon(QIcon(":/resources/settings.png"));
	ui->nextBtn->setText("");
	ui->nextBtn->setIcon(QIcon(":/resources/arrow-right.png"));
	ui->viewHighlightsBtn->setText("");
	ui->viewHighlightsBtn->setIcon(QIcon(":/resources/view-highlights.png"));

	// Style misc.
	ui->bookInfoLabel->setStyleSheet("font-style: italic");

	// Making text selectable
	ui->text->setTextInteractionFlags(Qt::TextSelectableByMouse);

	// Font misc.
	int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Italic.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont crimson(family);

	// Stylesheet + misc.
	UI::applyStyle(*this, "eink");

	// We have to get the file's path
	if (global::reader::skipOpenDialog == true) {
		if (checkconfig("/tmp/suspendBook") == true) {
			wakeFromSleep = true;
			// Prevent from opening the Reader framework next time unless the condition is reset
			writeFile("/tmp/suspendBook", "false");
			book_file = "/inkbox/book/book.txt";
		} else if (global::reader::bookFile.isEmpty() == false) {
			book_file = global::reader::bookFile;
			global::reader::bookFile = "";
		}
	} else {
		if (checkconfig("/opt/inkbox_genuine") == true) {
			if (checkconfig("/external_root/run/encfs_mounted") == true) {
				QDir::setCurrent("/mnt/onboard/onboard/encfs-decrypted");
			} else {
				QDir::setCurrent("/mnt/onboard/onboard");
			}
			QFileDialog *dialog = new QFileDialog(this);
			// https://forum.qt.io/topic/29471/solve-how-to-show-qfiledialog-at-center-position-screen/4
			QDesktopWidget desk;
			QRect screenres = desk.screenGeometry(0);
			dialog->setGeometry(QRect(screenres.width() / 4, screenres.height() / 4, screenres.width() / 2, screenres.height() / 2));

			UI::applyStyle(*dialog, "eink");

			book_file = dialog->getOpenFileName(dialog, tr("Open File"), QDir::currentPath());

			if (!book_file.isEmpty()) {
				setDefaultWorkDir();
			} else {
				// User clicked "Cancel" button
				// Restarting InkBox
				setDefaultWorkDir();
				QProcess process;
				process.startDetached("inkbox", QStringList());
				qApp->quit();
			}
		} else {
			if (checkconfig("/external_root/run/encfs_mounted") == true) {
				QDir::setCurrent("/mnt/onboard/onboard/encfs-decrypted");
			} else {
				QDir::setCurrent("/mnt/onboard/onboard");
			}
			QDir::setCurrent("/mnt/onboard");
			QFileDialog *dialog = new QFileDialog(this);
			// https://forum.qt.io/topic/29471/solve-how-to-show-qfiledialog-at-center-position-screen/4
			QDesktopWidget desk;
			QRect screenres = desk.screenGeometry(0);
			dialog->setGeometry(QRect(screenres.width() / 4, screenres.height() / 4, screenres.width() / 2, screenres.height() / 2));

			UI::applyStyle(*dialog, "eink");

			book_file = dialog->getOpenFileName(dialog, tr("Open File"), QDir::currentPath());

			if (!book_file.isEmpty()) {
				setDefaultWorkDir();
			} else {
				// User clicked "Cancel" button
				// Restarting InkBox
				setDefaultWorkDir();
				QProcess process;
				process.startDetached("inkbox", QStringList());
				qApp->quit();
			}
		}
	}
	log("Opening file '" + book_file + "'", className);

	// Writing book path to file
	writeFile("/tmp/inkboxBookPath", book_file);

	// Calling InkBox daemon (ibxd) via FIFO interface to run bookconfig_mount
	if (!book_file.isEmpty()) {
		if (checkconfig(".config/16-global_reading_settings/config") == false) {
			global::reader::globalReadingSettings = false;
			writeFile("/opt/ibxd", "bookconfig_mount\n");
			// Callback handler to wait until bookconfig_mount has finished execution
			while (true) {
				if (QFile::exists("/inkbox/bookConfigSetUp")) {
					QFile::remove("/inkbox/bookConfigSetUp");
					setupLocalSettingsEnvironment();
					break;
				}
			}
		} else {
			global::reader::globalReadingSettings = true;
		}
	}

	// Custom settings
	// Brightness
	QTimer::singleShot(0, this, SLOT(setCinematicBrightnessWarmthSlot()));
	// Font
	global::reader::font = readFile(".config/04-book/font");
	if (global::reader::font == "u001") {
		ui->fontChooser->setCurrentText("Univers (u001)");
	} else {
		ui->fontChooser->setCurrentText(global::reader::font);
	}
	// Night mode
	if (device->supportsNightmode()) {
		if (checkconfig(".config/10-dark_mode/config") == true) {
			log("Setting night mode to ON", className);
			writeFile("/tmp/invertScreen", "y");
			ui->nightModeBtn->setText("");
			ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-full.png"));
			isNightModeActive = true;
		} else {
			log("Setting night mode to OFF", className);
			writeFile("/tmp/invertScreen", "n");
			ui->nightModeBtn->setText("");
			ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-empty.png"));
			isNightModeActive = false;
		}
	} else {
		log("Night mode disabled by software", className);
		ui->line_7->hide();
		ui->line_7->deleteLater();
		ui->nightModeBtn->hide();
		ui->nightModeBtn->deleteLater();
	}

	Device::ScreenSize screenSize = device->getScreenSize();
	if (screenSize == Device::ScreenSize::VERY_LARGE) {
		ui->nextBtn->setStyleSheet("padding: 13.5px");
		ui->previousBtn->setStyleSheet("padding: 13.5px");
		ui->optionsBtn->setStyleSheet("padding: 13.5px");
	} else if (screenSize == Device::ScreenSize::LARGE) {
		ui->nextBtn->setStyleSheet("padding: 12.5px");
		ui->previousBtn->setStyleSheet("padding: 12.5px");
		ui->optionsBtn->setStyleSheet("padding: 12.5px");
	} else {
		ui->nextBtn->setStyleSheet("padding: 10px");
		ui->previousBtn->setStyleSheet("padding: 10px");
		ui->optionsBtn->setStyleSheet("padding: 10px");
	}
	ui->sizeValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
	ui->lineSpacingValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
	ui->marginsValueLabel->setStyleSheet("font-size: 9pt; font-weight: bold");
	ui->homeBtn->setStyleSheet("font-size: 9pt; padding: 5px");
	ui->brightnessBtn->setStyleSheet("font-size: 9pt; padding: 5px");
	ui->fontChooser->setStyleSheet("font-size: 9pt");
	ui->gotoBtn->setStyleSheet("font-size: 9pt; padding: 9px; font-weight: bold; background: lightGrey");
	ui->pageNumberLabel->setFont(QFont("Source Serif Pro"));
	ui->viewHighlightsBtn->setStyleSheet("padding: 9px");

	// Hiding the menubar + definition widget + brightness button + buttons bar widget
	ui->menuWidget->setVisible(false);
	if (device->supportsBrightness()) {
		ui->brightnessBtn->setVisible(true);
		ui->line_15->setVisible(true);
	} else {
		ui->brightnessBtn->setVisible(false);
		ui->brightnessBtn->deleteLater();
		ui->line_15->setVisible(false);
		ui->line_15->deleteLater();
	}
	ui->menuBarWidget->setVisible(false);
	ui->buttonsBarWidget->setVisible(false);
	ui->pdfScaleWidget->setVisible(false);
	ui->wordWidget->setVisible(false);
	if (checkconfig(".config/11-menubar/sticky") == true) {
		ui->menuWidget->setVisible(true);
		ui->statusBarWidget->setVisible(true);
	} else {
		ui->menuWidget->setVisible(false);
		ui->statusBarWidget->setVisible(false);
	}
	ui->pageWidget->hide();
	if (pdf_file_match(book_file) == true) {
		ui->line_4->setLineWidth(2);
	}

	// Topbar widget / book info
	ui->topbarStackedWidget->setVisible(true);
	showTopbarWidget = true;
	ui->bookInfoLabel->setFont(crimson);

	// Defining pixmaps
	// Getting the screen's size
	float sW = QGuiApplication::screens()[0]->size().width();
	float sH = QGuiApplication::screens()[0]->size().height();
	// Defining what the icons' size will be
	if (checkconfig("/opt/inkbox_genuine") == true) {
		float stdIconHeight = sW / 16;
		float stdIconWidth = sW / 16;

		if (screenSize == Device::ScreenSize::VERY_LARGE) {
			stdIconWidth = sW / 19;
			stdIconHeight = sH / 19;
		}
		QPixmap chargingPixmap(":/resources/battery_charging.png");
		scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap fullPixmap(":/resources/battery_full.png");
		scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap halfPixmap(":/resources/battery_half.png");
		scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap emptyPixmap(":/resources/battery_empty.png");
		scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	} else {
		float stdIconWidth = sW / 19;
		float stdIconHeight = sH / 19;
		QPixmap chargingPixmap(":/resources/battery_charging.png");
		scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap fullPixmap(":/resources/battery_full.png");
		scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap halfPixmap(":/resources/battery_half.png");
		scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap emptyPixmap(":/resources/battery_empty.png");
		scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	// Checking if there is a page refresh setting set
	if (readFile(".config/04-book/refresh").isEmpty()) {
		// Writing the default, refresh every 3 pages
		writeFile(".config/04-book/refresh", "3");
	}
	pageRefreshSetting = readFile(".config/04-book/refresh").toInt();
	// Checking if that config option was set to "Never refresh"...
	if (pageRefreshSetting == -1) {
		log("Setting page refresh to 'disabled'", className);
		neverRefresh = true;
	} else {
		log("Setting page refresh to each " + QString::number(pageRefreshSetting) + " pages", className);
		// Safety measure
		neverRefresh = false;
	}

	// Clock setting to show seconds + battery level
	// TODO: we have similar codeblocks throughout the codebase. It would make sense to spin this off into a subfile that can be reused
	if (checkconfig(".config/02-clock/config") == true) {
		QTimer *t = new QTimer(this);
		t->setInterval(500);
		connect(t, &QTimer::timeout, [&]() {
			QString time = QTime::currentTime().toString("hh:mm:ss");
			ui->batteryLabel->setText(QString(device->getBatteryLevel()));
			ui->timeLabel->setText(time);
		});
		t->start();
	} else {
		QTimer *t = new QTimer(this);
		t->setInterval(500);
		connect(t, &QTimer::timeout, [&]() {
			QString time = QTime::currentTime().toString("hh:mm");
			ui->batteryLabel->setText(QString(device->getBatteryLevel()));
			ui->timeLabel->setText(time);
		});
		t->start();
	}

	// Checking if we're waking from sleep and have lockscreen enabled; if so, do nothing there because the book should already have been parsed
	if (wakeFromSleep != true) {
		// Remount tmpfs
		writeFile("/inkbox/remount", "true");

		// Counting number of parsed files
		split_total = setup_book(book_file, 0, true);
		split_files_number = split_total;
		split_total = split_total - 1;

		writeconfig_pagenumber(false);
	} else {
		// Retrieve split_total from tmpfs
		split_total = readFile("/tmp/inkboxPageNumber").toInt();
		setup_book(book_file, 0, true);
	}

	// Get text; no need to do it multiple times for ePUB books
	if (is_epub == false && is_pdf == false && is_image == false) {
		setDefaultWorkDir();
		if (global::reader::globalReadingSettings == false) {
			QString splitTotalPageNumber = readFile(".config/A-page_number/config");
			if (!splitTotalPageNumber.isEmpty()) {
				split_total = splitTotalPageNumber.toInt();
			}
		}
		setup_book(book_file, split_total, true);
	}

	// Display text
	// If needed, show scroll bar when rendering engine isn't doing its job properly
	if (checkconfig(".config/14-reader_scrollbar/config") == true) {
		ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	} else {
		ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

	// Line spacing, margins and text alignment
	QString lineSpacingString = readFile(".config/04-book/line_spacing");
	if (!lineSpacingString.isEmpty()) {
		global::reader::lineSpacing = lineSpacingString.toInt();
	}

	QString marginsString = readFile(".config/04-book/margins");
	if (!marginsString.isEmpty()) {
		global::reader::margins = marginsString.toInt();
	}

	QString alignment = readFile(".config/04-book/alignment");
	if (alignment == "Left") {
		global::reader::textAlignment = 0;
	} else if (alignment == "Center") {
		global::reader::textAlignment = 1;
	} else if (alignment == "Right") {
		global::reader::textAlignment = 2;
	} else if (alignment == "Justify") {
		global::reader::textAlignment = 3;
	} else {
		alignment = "Left";
		global::reader::textAlignment = 0;
	}
	log("Setting text alignment to '" + alignment + "'", className);

	// Don't ask me why it doesn't work without that QTimer
	QTimer::singleShot(0, this, SLOT(setInitialTextProperties()));

	// Wheeee!
	if (is_epub == true) {
		ui->graphicsView->hide();
		ui->graphicsView->deleteLater();
		ui->text->setText(epubPageContent);
		global::reader::currentViewportText = ui->text->toHtml();
	} else if (is_pdf == true) {
		ui->text->hide();
		ui->text->deleteLater();
		ui->line_20->hide();
		ui->line_20->deleteLater();
		ui->viewHighlightsBtn->hide();
		ui->viewHighlightsBtn->deleteLater();
		setupPng();
	} else if (is_image == true) {
		ui->text->hide();
		ui->text->deleteLater();
		ui->line_20->hide();
		ui->line_20->deleteLater();
		ui->viewHighlightsBtn->hide();
		ui->viewHighlightsBtn->deleteLater();
		setupPng();
	} else {
		ui->graphicsView->hide();
		ui->graphicsView->deleteLater();
		global::reader::currentViewportText = ui->text->toHtml();
		ui->text->setText(ittext);
	}

	// Topbar info widget
	if (is_epub == true) {
		QString bookCreator = findEpubMetadata(book_file, "creator");
		QString bookTitle = findEpubMetadata(book_file, "title");
		bookCreator = bookCreator.trimmed();
		bookTitle = bookTitle.trimmed();

		QString infoLabelContent = bookCreator;
		infoLabelContent.append(" ― ");
		infoLabelContent.append(bookTitle);
		int infoLabelLength = infoLabelContent.length();
		int infoLabelDefinedLength;
		if (screenSize == Device::ScreenSize::SMALL) {
			infoLabelDefinedLength = 35;
		} else {
			infoLabelDefinedLength = 50;
		}

		if (infoLabelLength <= infoLabelDefinedLength) {
			ui->bookInfoLabel->setWordWrap(false);
		} else {
			ui->bookInfoLabel->setWordWrap(true);
		}
		ui->bookInfoLabel->setText(infoLabelContent);
	} else {
		QString bookReadRelativePath;
		if (wakeFromSleep == true) {
			bookReadRelativePath = readFile("/tmp/inkboxBookPath").split("/").last();
		} else {
			bookReadRelativePath = book_file.split("/").last();
		}

		int infoLabelDefinedLength;
		if (screenSize == Device::ScreenSize::SMALL) {
			infoLabelDefinedLength = 35;
		} else {
			infoLabelDefinedLength = 50;
		}

		if (bookReadRelativePath <= infoLabelDefinedLength) {
			ui->bookInfoLabel->setWordWrap(false);
		} else {
			ui->bookInfoLabel->setWordWrap(true);
		}
		ui->bookInfoLabel->setText(bookReadRelativePath);
	}

	// Clean up
	writeFile("/inkbox/remount", "true");

	// Way to tell shell scripts that we're in the Reader framework
	writeFile("/tmp/inkboxReading", "true");

	// Maintain a 'Recent books' list
	QJsonObject recentBooksObject;
	if (QFile::exists(global::localLibrary::recentBooksDatabasePath)) {
		log("Reading recent books database", className);
		QFile recentBooksDatabase(global::localLibrary::recentBooksDatabasePath);
		QByteArray recentBooksData;
		if (recentBooksDatabase.open(QIODevice::ReadOnly)) {
			recentBooksData = recentBooksDatabase.readAll();
			recentBooksDatabase.close();
		} else {
			QString function = __func__;
			log(function + ": Failed to open recent books database file for reading at '" + recentBooksDatabase.fileName() + "'", className);
		}
		QJsonObject mainJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(recentBooksData))).object();
		for (int i = 1; i <= global::homePageWidget::recentBooksNumber; i++) {
			QString objectName = "Book" + QString::number(i);
			QJsonObject jsonObject = mainJsonObject[objectName].toObject();
			if (i == 1) {
				if (jsonObject.value("BookPath").toString() != book_file) {
					// Circular buffer
					for (int i = global::homePageWidget::recentBooksNumber; i >= 2; i--) {
						mainJsonObject["Book" + QString::number(i)] = mainJsonObject["Book" + QString::number(i - 1)];
					}
					jsonObject.insert("BookPath", QJsonValue(book_file));
					mainJsonObject[objectName] = jsonObject;
				}
			}
		}
		recentBooksObject = mainJsonObject;
	} else {
		QJsonObject mainJsonObject;
		QJsonObject firstJsonObject;
		firstJsonObject.insert("BookPath", QJsonValue(book_file));
		mainJsonObject["Book1"] = firstJsonObject;

		for (int i = 2; i <= global::homePageWidget::recentBooksNumber; i++) {
			QJsonObject jsonObject;
			jsonObject.insert("BookPath", QJsonValue(""));
			mainJsonObject["Book" + QString::number(i)] = jsonObject;
		}
		recentBooksObject = mainJsonObject;
	}
	QFile::remove(global::localLibrary::recentBooksDatabasePath);
	writeFile(global::localLibrary::recentBooksDatabasePath, qCompress(QJsonDocument(recentBooksObject).toJson()).toBase64());

	// USB mass storage prompt
	if (global::reader::startUsbmsPrompt == true) {
		QTimer *usbmsPrompt = new QTimer(this);
		usbmsPrompt->setInterval(500);
		connect(usbmsPrompt, &QTimer::timeout, [&]() {
			if (checkconfig("/opt/inkbox_genuine") == true) {
				if (global::usbms::showUsbmsDialog != true) {
					if (device->isUSBPluggedIn() != usbmsStatus) {
						global::usbms::showUsbmsDialog = true;
					}
				} else {
					usbmsStatus = device->isUSBPluggedIn();
					if (usbmsStatus == false) {
						// Loop again...
						;
					} else {
						// An USB cable is connected!
						openUsbmsDialog();
					}
				}
			} else {
				// Do nothing, we're running along with Nickel & friends...
				;
			}
		});
		usbmsPrompt->start();
	}

	// Battery watchdog
	if (global::reader::startBatteryWatchdog == true) {
		QTimer *t = new QTimer(this);
		t->setInterval(2000);
		connect(t, &QTimer::timeout, [&]() {
			// Checking if battery level is low
			if (global::battery::showCriticalBatteryAlert) {
				if (device->isBatteryCritical() && !device->isUSBPluggedIn()) {
					log("Warning! Battery is at a critical charge level!", className);
					openCriticalBatteryAlertWindow();
				}
			}

			if (global::battery::showLowBatteryDialog) {
				if (device->isBatteryLow() && !device->isUSBPluggedIn()) {
					if (global::battery::batteryAlertLock == true) {
						;
					} else {
						log("Warning! Battery is low!", className);
						openLowBatteryDialog();
					}
				}
			}
		});
		if (device->hasBattery()) {
			t->start();
		}
	}

	// Pages number info label
	if (is_pdf == true) {
		getTotalPdfPagesNumber();
	}
	// The total pages number is already known for ePUBs at this point
	setupPageWidget();

	// Reading settings auto-save timer
	if (global::reader::globalReadingSettings == false) {
		QTimer *saveSettingsTimer = new QTimer(this);
		saveSettingsTimer->setInterval(900000); // 15 minutes
		connect(saveSettingsTimer, SIGNAL(timeout()), this, SLOT(saveReadingSettings()));
		saveSettingsTimer->start();
	}
}

ReaderWidget::~ReaderWidget() {
	delete ui;
}

int ReaderWidget::setup_book(QString book, int i, bool run_parser) {
	// Parse eBook
	if (remount == true) {
		QString mount_prog("sh");
		QStringList mount_args;
		mount_args << "split.sh";
		QProcess *mount_proc = new QProcess();
		mount_proc->start(mount_prog, mount_args);
		mount_proc->waitForFinished();
		mount_proc->deleteLater();
		remount = false;
	} else {
		writeFile("/inkbox/remount", "false");
		QString mount_prog("sh");
		QStringList mount_args;
		mount_args << "split.sh";
		QProcess *mount_proc = new QProcess();
		mount_proc->start(mount_prog, mount_args);
		mount_proc->waitForFinished();
		mount_proc->deleteLater();
	}

	if (filematch_ran == false) {
		if (epub_file_match(book) == true) {
			QFile::remove("/run/book.epub");
			QFile::copy(book, "/run/book.epub");

			getTotalEpubPagesNumber();

			filematch_ran = true;
			is_epub = true;
		} else if (pdf_file_match(book) == true) {
			getPdfOrientation(book);
			QString pdfProg("/usr/local/bin/mutool");
			QStringList pdfArgs;
			convertMuPdfVars(1, true);
			pdfArgs << "convert"
					<< "-F"
					<< "png"
					<< "-O"
					<< "width=" + mupdf::pdf::width_qstr + ",height=" + mupdf::pdf::height_qstr << "-o"
					<< "/run/page.png" << book_file << mupdf::pdf::pdfPageNumber_qstr;
			QProcess *pdfProc = new QProcess();
			pdfProc->start(pdfProg, pdfArgs);
			pdfProc->waitForFinished();
			pdfProc->deleteLater();

			filematch_ran = true;
			is_pdf = true;
		} else if (image_file_match(book) == true) {
			ui->previousBtn->hide();
			ui->nextBtn->hide();
			ui->line->hide();
			ui->line_3->hide();
			ui->previousBtn->deleteLater();
			ui->nextBtn->deleteLater();
			ui->line->deleteLater();
			ui->line_3->deleteLater();

			writeFile("/tmp/inkboxImagePath", book);
			writeFile("/opt/ibxd", "inkbox_convert_image\n");

			while (true) {
				if (QFile::exists("/inkbox/convertImageDone")) {
					if (checkconfig("/inkbox/convertImageDone") == true) {
						filematch_ran = true;
						is_image = true;
						break;
					} else {
						showToast("Failed to display image");
						break;
					}
					QFile::remove("/inkbox/convertImageDone");
				}
			}

			filematch_ran = true;
			is_image = true;
		} else {
			// This is likely not an ePUB, a PDF or a picture.
			// Copying book specified in the function call
			QFile::copy(book, "/inkbox/book/book.txt");

			filematch_ran = true;
			is_epub = false;
		}
	}

	// Checking whether or not the user has defined an option for the number of words per page; if not, then setting the default.
	// NOTE: This is only for plain text files.
	setDefaultWorkDir();
	if (readFile(".config/07-words_number/config").isEmpty()) {
		writeFile(".config/07-words_number/config", "100");
	}
	QString wordsNumberConfig = readFile(".config/07-words_number/config");

	// Parsing file
	if (is_epub == true) {
		if (run_parser == true) {
			if (filematch_ran == true) {
				QString epubProg("sh");
				QStringList epubArgs;
				convertMuPdfVars(0, false);
				epubArgs << "/mnt/onboard/.adds/inkbox/epub.sh"
						 << "0"
						 << "0"
						 << "0" << mupdf::epub::epubPageNumber_qstr;
				QProcess *epubProc = new QProcess();
				epubProc->start(epubProg, epubArgs);
				epubProc->waitForFinished();
				epubProc->deleteLater();
			}
		}
	} else if (is_pdf == true) {
		if (run_parser == true) {
			if (filematch_ran == true) {
				QString pdfProg("/usr/local/bin/mutool");
				QStringList pdfArgs;
				convertMuPdfVars(1, true);
				pdfArgs << "convert"
						<< "-F"
						<< "png"
						<< "-O"
						<< "width=" + mupdf::pdf::width_qstr + ",height=" + mupdf::pdf::height_qstr << "-o"
						<< "/run/page.png" << book_file << mupdf::pdf::pdfPageNumber_qstr;
				QProcess *pdfProc = new QProcess();
				pdfProc->start(pdfProg, pdfArgs);
				pdfProc->waitForFinished();
				pdfProc->deleteLater();
			}
		}
	} else if (is_image == true) {
		// Do nothing, since images won't have pages
		;
	} else {
		QString parse_prog("/mnt/onboard/.adds/inkbox/system/bin/split-txt");
		QStringList parse_args;
		parse_args << wordsNumberConfig;
		QProcess *parse_proc = new QProcess();
		parse_proc->start(parse_prog, parse_args);
		parse_proc->waitForFinished();
		parse_proc->deleteLater();
	}

	// Reading files
	if (is_epub == false && is_pdf == false && is_image == false) {
		if (run_parser == true) {
			QDirIterator it("/inkbox/book/split");
			while (it.hasNext()) {
				QFile f(it.next());
				f.open(QIODevice::ReadOnly);
				content << f.readAll();
				f.close();
			}
			// These characters are replaced for consistency with ePUB text handling, highlighting and text alignment.
			content[i].replace("\n", "<br>");
			content[i].replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
			ittext = content[i];
			return content.size();
		} else {
			content[i].replace("\n", "<br>");
			content[i].replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;");
			ittext = content[i];
		}
	} else {
		QFile epubPage("/inkbox/book/page");
		epubPage.open(QIODevice::ReadOnly);
		QTextStream in(&epubPage);
		epubPageContent = in.readAll();
		epubPage.close();
	}
	return 0;
}

void ReaderWidget::checkwords() {
	QFile words_list(".config/06-words/config");
	words_list.open(QIODevice::ReadWrite);
	QTextStream in(&words_list);
	words = in.readAll();
	words_list.close();
}

bool ReaderWidget::epub_file_match(QString file) {
	QString fileExt = file.right(4);

	if (fileExt == "epub" or fileExt == "EPUB") {
		log("Book file format: ePUB", className);
		writeFile("/inkbox/bookIsEpub", "true");
		return true;
	} else {
		writeFile("/inkbox/bookIsEpub", "false");
		return false;
	}
}

void ReaderWidget::dictionary_lookup(string word, QString first_letter, int position) {
	log("Dictionary lookup requested for word '" + QString::fromStdString(word) + "', position " + QString::number(position), className);
	ofstream fhandler;
	fhandler.open("/inkbox/dictionary/word");
	fhandler << word;
	fhandler.close();

	QDir::setCurrent("dictionary");
	QDir::setCurrent(first_letter);
	QString lookup_prog("sh");
	QStringList lookup_args;
	QString position_str = QString::number(position);
	lookup_args << "../scripts/lookup.sh" << position_str;
	QProcess *lookup_proc = new QProcess();
	lookup_proc->start(lookup_prog, lookup_args);
	lookup_proc->waitForFinished();
	lookup_proc->deleteLater();

	QFile definition_file("/inkbox/dictionary/definition");
	definition_file.open(QIODevice::ReadWrite);
	QTextStream in(&definition_file);
	definition = in.readAll();
	definition = definition.remove(QRegExp("[\n]"));
	if (definition == "No definition found.") {
		nextdefinition_lock = true;
	} else {
		nextdefinition_lock = false;
	}
	definition_file.close();

	setDefaultWorkDir();
}

void ReaderWidget::save_word(string word, bool remove) {
	if (remove == false) {
		QFile words(".config/06-words/config");
		words.open(QIODevice::ReadWrite);
		QTextStream in(&words);
		QString words_list = in.readAll();
		string words_list_str = words_list.toStdString();
		words.close();

		log("Saving word '" + QString::fromStdString(word) + "' in Saved Words list", className);
		ofstream fhandler;
		fhandler.open(".config/06-words/config");
		fhandler << words_list_str << word << "\n";
		fhandler.close();
	} else {
		ofstream fhandler;
		fhandler.open(".config/06-words/config");
		fhandler << word;
		fhandler.close();
	}
}

void ReaderWidget::on_nextBtn_clicked() {
	// ui->nextBtn->setEnabled(false);
	if (is_epub == false and is_pdf == false) {
		if (split_total - 1 == 1 or split_total - 1 == 0) {
			showToast("You've reached the end of the document");
		} else {
			split_total = split_total - 1;
			log("'Next' button clicked", className);

			setup_book(book_file, split_total, false);
			ui->text->setText("");
			ui->text->setText(ittext);
			global::reader::currentViewportText = ui->text->toHtml();

			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	} else if (is_epub == true) {
		if (mupdf::epub::epubPageNumber + 1 > totalPagesInt) {
			showToast("You've reached the end of the document");
		} else {
			mupdf::epub::epubPageNumber = mupdf::epub::epubPageNumber + 1;
			log("'Next' button clicked: going to page " + QString::number(mupdf::epub::epubPageNumber), className);
			setup_book(book_file, mupdf::epub::epubPageNumber, true);
			ui->text->setText("");
			ui->text->setText(epubPageContent);
			global::reader::currentViewportText = ui->text->toHtml();

			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	} else if (is_pdf == true) {
		if (mupdf::pdf::pdfPageNumber + 1 > totalPagesInt) {
			showToast("You've reached the end of the document");
		} else {
			mupdf::pdf::pdfPageNumber = mupdf::pdf::pdfPageNumber + 1;
			log("'Next' button clicked: going to page " + QString::number(mupdf::pdf::pdfPageNumber), className);
			if (ui->pdfScaleSlider->value() != 1) {
				mupdf::convertRelativeValues = true;
			}
			setup_book(book_file, mupdf::pdf::pdfPageNumber, true);
			setupPng();

			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
		}
	}
	setupPageWidget();
	refreshScreen();
	// ui->nextBtn->setEnabled(true);
}

void ReaderWidget::on_previousBtn_clicked() {
	// ui->previousBtn->setEnabled(false);
	if (is_epub == false and is_pdf == false) {
		// Making sure we won't encounter a "List index out of range" error ;)
		if (split_total >= split_files_number - 1) {
			showToast("No previous page");
		} else {
			split_total = split_total + 1;
			log("'Previous' button clicked", className);

			setup_book(book_file, split_total, false);
			ui->text->setText("");
			ui->text->setText(ittext);
			global::reader::currentViewportText = ui->text->toHtml();

			// We always increment pagesTurned regardless whether we press the Previous or Next button or not
			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	} else if (is_pdf == true) {
		if (mupdf::pdf::pdfPageNumber - 1 <= 0) {
			showToast("No previous page");
		} else {
			mupdf::pdf::pdfPageNumber = mupdf::pdf::pdfPageNumber - 1;
			log("'Previous' button clicked: going to page " + QString::number(mupdf::pdf::pdfPageNumber), className);
			if (ui->pdfScaleSlider->value() != 1) {
				mupdf::convertRelativeValues = true;
			}
			setup_book(book_file, mupdf::pdf::pdfPageNumber, true);
			setupPng();

			// We always increment pagesTurned regardless whether we press the Previous or Next button or not
			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
		}
	} else {
		if (mupdf::epub::epubPageNumber - 1 <= 0) {
			showToast("No previous page");
		} else {
			mupdf::epub::epubPageNumber = mupdf::epub::epubPageNumber - 1;
			log("'Previous' button clicked: going to page " + QString::number(mupdf::epub::epubPageNumber), className);
			setup_book(book_file, mupdf::epub::epubPageNumber, true);
			ui->text->setText("");
			ui->text->setText(epubPageContent);
			global::reader::currentViewportText = ui->text->toHtml();

			// We always increment pagesTurned regardless whether we press the Previous or Next button or not
			pagesTurned = pagesTurned + 1;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	}
	setupPageWidget();
	refreshScreen();
	// ui->previousBtn->setEnabled(true);
}

void ReaderWidget::refreshScreen() {
	if (neverRefresh == true) {
		// Do nothing; "Never refresh" was set
		;
	} else {
		if (pagesTurned >= pageRefreshSetting) {
			// Refreshing the screen
			this->repaint();
			// Reset count
			pagesTurned = 0;
		}
	}
}

void ReaderWidget::on_optionsBtn_clicked() {
	Device::ScreenSize screenSize = Device::getSingleton()->getScreenSize();
	log("'Options' button clicked", className);
	if (menubar_shown == true) {
		menubar_hide();
		if (screenSize == Device::ScreenSize::VERY_LARGE) {
			ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 13.5px");
		} else if (screenSize == Device::ScreenSize::LARGE) {
			ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 12.5px");
		} else {
			ui->optionsBtn->setStyleSheet("background: white; color: black; padding: 10px");
		}
		ui->optionsBtn->setIcon(QIcon(":/resources/settings.png"));
		// The Glo HD (N437) has a newer platform plugin that doesn't need this
		// TODO: figure out what this actually does
		/*if(global::deviceID != "n437\n" or global::deviceID != "n306\n" or global::deviceID != "n249\n") {
			QTimer::singleShot(500, this, SLOT(repaint()));
		}*/
		menubar_shown = false;
	} else {
		menubar_show();
		if (screenSize == Device::ScreenSize::VERY_LARGE) {
			ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 13.5px");
		} else if (screenSize == Device::ScreenSize::LARGE) {
			ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 12.5px");
		} else {
			ui->optionsBtn->setStyleSheet("background: black; color: white; padding: 10px");
		}
		ui->optionsBtn->setIcon(QIcon(":/resources/settings-inverted.png"));
		this->repaint();
		menubar_shown = true;
	}
}

void ReaderWidget::on_homeBtn_clicked() {
	log("Returning to Home screen", className);
	// We're leaving reading mode
	writeFile("/tmp/inkboxReading", "false");
	// Remount tmpfs
	writeFile("/inkbox/remount", "true");
	// Specify cinematic brightness mode
	writeFile("/tmp/inkbox-cinematicBrightness_auto", "true");

	// Relaunching process
	quit_restart();
}

void ReaderWidget::on_fontChooser_currentIndexChanged(const QString &arg1) {
	log("Setting font to '" + arg1 + "'", className);
	global::reader::font = arg1;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
}

void ReaderWidget::on_alignLeftBtn_clicked() {
	log("Setting text alignment to 'Left'", className);
	global::reader::textAlignment = 0;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	writeFile(".config/04-book/alignment", "Left");
}

void ReaderWidget::on_alignCenterBtn_clicked() {
	log("Setting text alignment to 'Center'", className);
	global::reader::textAlignment = 1;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	writeFile(".config/04-book/alignment", "Center");
}

void ReaderWidget::on_alignRightBtn_clicked() {
	log("Setting text alignment to 'Right'", className);
	global::reader::textAlignment = 2;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	writeFile(".config/04-book/alignment", "Right");
}

void ReaderWidget::on_alignJustifyBtn_clicked() {
	log("Setting text alignment to 'Justify'", className);
	global::reader::textAlignment = 3;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	writeFile(".config/04-book/alignment", "Justify");
}

void ReaderWidget::setTextProperties(int alignment, int lineSpacing, int margins, QString font, int fontSize) {
	// Don't try to improve this
	// I have spent more time on it than I would care to admit

	// Alignment
	/*
	 * 0 - Left
	 * 1 - Center
	 * 2 - Right
	 * 3 - Justify
	 */

	// Selection color
	ui->text->setStyleSheet("QTextEdit { selection-background-color: white; color: white; }");
	// Font
	{
		if (font == "Crimson Pro") {
			// As adding Crimson Pro to the default fonts bundled along with the Qt libs breaks the general u001/Inter homogeneity, it is incorporated on-demand here.
			{
				QString family;
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Regular.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Italic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Bold.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-BoldItalic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				ui->text->setFont(QFont(family));
			}
			writeFile(".config/04-book/font", "Crimson Pro");
		} else if (font == "Bitter") {
			{
				QString family;
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/Bitter-Medium.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/Bitter-MediumItalic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/Bitter-Bold.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/Bitter-BoldItalic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				ui->text->setFont(QFont(family));
			}
			writeFile(".config/04-book/font", "Bitter");
		} else if (font == "Ibarra Real Nova") {
			{
				QString family;
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/IbarraRealNova-Medium.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/IbarraRealNova-MediumItalic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/IbarraRealNova-Bold.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				{
					int id = QFontDatabase::addApplicationFont(":/resources/fonts/IbarraRealNova-BoldItalic.ttf");
					family = QFontDatabase::applicationFontFamilies(id).at(0);
				}
				ui->text->setFont(QFont(family));
			}
			writeFile(".config/04-book/font", "Ibarra Real Nova");
		} else if (font == "Univers (u001)" or font == "u001") {
			ui->text->setFont(QFont("u001"));
			writeFile(".config/04-book/font", "Univers (u001)");
		} else {
			ui->text->setFont(QFont(font));
			writeFile(".config/04-book/font", font);
		}
	}

	// Highlighting
	QString htmlText = global::reader::currentViewportText;
	QJsonObject jsonObject = getHighlightsForBook(book_file);
	int keyCount = 1;
	foreach (const QString &key, jsonObject.keys()) {
		if (keyCount <= 1) {
			keyCount++;
			continue;
		} else {
			QString highlight = jsonObject.value(key).toString();
			if (htmlText.contains(highlight)) {
				htmlText.replace(highlight, "<span style=\" background-color:#bbbbbb;\">" + highlight + "</span>");
			}
		}
		keyCount++;
	}
	htmlText.replace(QRegExp("font-family:'.*';"), "");
	ui->text->setHtml(htmlText);

	// Line spacing, margins
	setLineSpacing(lineSpacing, false);
	ui->lineSpacingSlider->setValue(lineSpacing);
	setMargins(margins, false);
	ui->marginsSlider->setValue(margins);

	textDialogLock = true;
	QTextCursor cursor = ui->text->textCursor();
	// Kudos to Qt for not implementing the opposite of the following function /)_-)
	ui->text->selectAll();
	// Text alignment
	if (alignment == 0) {
		ui->text->setAlignment(Qt::AlignLeft);
	} else if (alignment == 1) {
		ui->text->setAlignment(Qt::AlignHCenter);
	} else if (alignment == 2) {
		ui->text->setAlignment(Qt::AlignRight);
	} else if (alignment == 3) {
		ui->text->setAlignment(Qt::AlignJustify);
	}
	// Font size
	ui->text->setFontPointSize(fontSize);
	ui->text->setTextCursor(cursor);
	// Selection color
	ui->text->setStyleSheet("QTextEdit { selection-background-color: black; color: black; }");
	textDialogLock = false;
}

void ReaderWidget::menubar_show() {
	log("Showing menu bar", className);
	// Checking battery level and status, then displaying the relevant icon on batteryIconLabel
	if (Device::getSingleton()->isUSBPluggedIn()) {
		ui->batteryIconLabel->setPixmap(scaledChargingPixmap);
	} else {
		int batteryLevel = Device::getSingleton()->getBatteryLevel();
		if (batteryLevel >= 75 && batteryLevel <= 100) {
			ui->batteryIconLabel->setPixmap(scaledFullPixmap);
		}
		if (batteryLevel >= 25 && batteryLevel <= 74) {
			ui->batteryIconLabel->setPixmap(scaledHalfPixmap);
		}
		if (batteryLevel >= 0 && batteryLevel <= 24) {
			ui->batteryIconLabel->setPixmap(scaledEmptyPixmap);
		}
	}

	ui->menuWidget->setVisible(true);
	if (is_pdf == false && is_image == false) {
		ui->menuBarWidget->setVisible(true);
	} else {
		if (is_pdf == true) {
			ui->pdfScaleWidget->setVisible(true);
		}
		ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
	ui->buttonsBarWidget->setVisible(true);
	ui->statusBarWidget->setVisible(true);
	if (is_image == true) {
		ui->pageWidget->setVisible(false);
	} else {
		ui->pageWidget->setVisible(true);
	}

	menubar_shown = true;
}

void ReaderWidget::menubar_hide() {
	log("Hiding menu bar", className);

	if (is_pdf == false && is_image == false) {
		ui->menuBarWidget->setVisible(false);
	} else {
		ui->pdfScaleWidget->setVisible(false);
		if (checkconfig(".config/14-reader_scrollbar/config") == true) {
			ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		} else {
			ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}
	ui->buttonsBarWidget->setVisible(false);
	ui->pageWidget->setVisible(false);
	if (checkconfig(".config/11-menubar/sticky") == true) {
		ui->statusBarWidget->setVisible(true);
	} else {
		ui->statusBarWidget->setVisible(false);
		ui->menuWidget->setVisible(false);
	}
	menubar_shown = false;
}

void ReaderWidget::wordwidget_show() {
	log("Showing word widget", className);
	if (menubar_shown == true) {
		menubar_hide();
		ui->optionsBtn->hide();
		ui->line->hide();
		ui->wordWidget->setVisible(true);
	} else {
		ui->optionsBtn->hide();
		ui->line->hide();
		ui->wordWidget->setVisible(true);
	}
}

void ReaderWidget::wordwidget_hide() {
	log("Hiding word widget", className);
	ui->wordWidget->setVisible(false);
	ui->optionsBtn->setStyleSheet("background: white; color: black");
	ui->optionsBtn->show();
	ui->line->show();
	wordwidgetLock = false;
}

void ReaderWidget::on_infoCloseBtn_clicked() {
	wordwidget_hide();
	dictionary_position = 1;
	QString dictionary_position_str = QString::number(dictionary_position);
	ui->definitionStatusLabel->setText(dictionary_position_str);

	QTextCursor cursor = ui->text->textCursor();
	cursor.clearSelection();
	ui->text->setTextCursor(cursor);
}

void ReaderWidget::on_previousDefinitionBtn_clicked() {
	dictionary_position = dictionary_position - 1;
	if (dictionary_position <= 0) {
		dictionary_position = 1;
	} else {
		dictionary_lookup(selected_text_str, letter, dictionary_position);
		ui->definitionLabel->setText(definition);
		QString dictionary_position_str = QString::number(dictionary_position);
		ui->definitionStatusLabel->setText(dictionary_position_str);
	}
}

void ReaderWidget::on_nextDefinitionBtn_clicked() {
	dictionary_position = dictionary_position + 1;
	dictionary_lookup(selected_text_str, letter, dictionary_position);
	if (nextdefinition_lock == true) {
		dictionary_position = dictionary_position - 1;
	} else {
		ui->definitionLabel->setText(definition);
		QString dictionary_position_str = QString::number(dictionary_position);
		ui->definitionStatusLabel->setText(dictionary_position_str);
	}
}

void ReaderWidget::on_saveWordBtn_clicked() {
	if (checkconfig_match(".config/06-words/config", selected_text_str) == true) {
		log("Removing word '" + QString::fromStdString(selected_text_str) + "' from Saved Words list", className);
		checkwords();
		word = word.append("\n");
		words = words.replace(word, "");
		string words_std_string = words.toStdString();
		save_word(words_std_string, true);
		ui->saveWordBtn->setText("");
		ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
	} else {
		save_word(selected_text_str, false);
		ui->saveWordBtn->setText("");
		ui->saveWordBtn->setIcon(QIcon(":/resources/starred_star.png"));
	}
}

void ReaderWidget::on_sizeSlider_valueChanged(int value) {
	// Font size
	log("Setting font size to " + QString::number(value + global::reader::initialFontSize + 1) + " points", className);
	ui->sizeValueLabel->setText(QString::number(value + 1));

	global::reader::fontSize = global::reader::initialFontSize + value + 1;
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);

	writeFile(".config/04-book/size", QString::number(value));
}

void ReaderWidget::writeconfig_pagenumber(bool persistent) {
	// Saving the page number in tmpfs and in persistent storage if requested
	if (is_epub == true) {
		QString epubPageNumberStr = QString::number(mupdf::epub::epubPageNumber);
		writeFile("/tmp/inkboxPageNumber", epubPageNumberStr);
		if (persistent == true) {
			log("Writing page number config for page '" + QString::number(mupdf::epub::epubPageNumber) + "'", className);
			epubPageNumberStr.append("\n");
			writeFile(".config/A-page_number/config", epubPageNumberStr);
		}
	} else if (is_pdf == true) {
		QString pdfPageNumberStr = QString::number(mupdf::pdf::pdfPageNumber);
		writeFile("/tmp/inkboxPageNumber", pdfPageNumberStr);
		if (persistent == true) {
			log("Writing page number config for page '" + pdfPageNumberStr + "'", className);
			pdfPageNumberStr.append("\n");
			writeFile(".config/A-page_number/config", pdfPageNumberStr);
		}
	} else {
		QString splitTotalStr = QString::number(split_total);
		writeFile("/tmp/inkboxPageNumber", splitTotalStr);
		if (persistent == true) {
			log("Writing page number config for split total '" + splitTotalStr + "'", className);
			splitTotalStr.append("\n");
			writeFile(".config/A-page_number/config", splitTotalStr);
		}
	}
}

void ReaderWidget::quit_restart() {
	log("Restarting InkBox", className);

	// Saving current page number
	saveReadingSettings();

	// Cleaning bookconfig_mount mountpoint
	writeFile("/opt/ibxd", "bookconfig_unmount\n");

	// Restarting InkBox
	QProcess process;
	process.startDetached("inkbox", QStringList());
	qApp->quit();
}

void ReaderWidget::openLowBatteryDialog() {
	log("Showing low battery dialog", className);
	global::mainwindow::lowBatteryDialog = true;
	global::battery::batteryAlertLock = true;

	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	QApplication::processEvents();
}

void ReaderWidget::openCriticalBatteryAlertWindow() {
	log("Showing critical battery alert splash", className);
	global::battery::showCriticalBatteryAlert = true;
	global::battery::showLowBatteryDialog = false;

	alertWindow = new SplashAlertWidget();
	alertWindow->setAttribute(Qt::WA_DeleteOnClose);
	alertWindow->setGeometry(QRect(QPoint(0, 0), screen()->geometry().size()));
	alertWindow->show();
}

void ReaderWidget::convertMuPdfVars(int fileType, bool convertAll) {
	/* fileType can be:
	 * 0: ePUB
	 * 1: PDF
	 * 2: Image
	 */
	if (fileType == 0) {
		if (convertAll == 1) {
			setPageStyle(0);
			mupdf::epub::fontSize = 12;
			mupdf::epub::fontSize_qstr = QString::number(mupdf::epub::fontSize);
			mupdf::epub::width_qstr = QString::number(mupdf::epub::width);
			mupdf::epub::height_qstr = QString::number(mupdf::epub::height);
		}
		if (global::reader::globalReadingSettings == false) {
			if (goToSavedPageDone == false) {
				mupdf::epub::epubPageNumber = readFile(".config/A-page_number/config").toInt();
				goToSavedPageDone = true;
			}
		}
		if (mupdf::epub::epubPageNumber <= 0) {
			mupdf::epub::epubPageNumber = 1;
		}
		mupdf::epub::epubPageNumber_qstr = QString::number(mupdf::epub::epubPageNumber);
	} else if (fileType == 1) {
		setPageStyle(1);
		mupdf::pdf::width = defaultPdfPageWidth;
		mupdf::pdf::height = defaultPdfPageHeight;
		if (mupdf::convertRelativeValues == true) {
			// For scaling
			mupdf::pdf::width_qstr = QString::number(mupdf::pdf::relativeWidth);
			mupdf::pdf::height_qstr = QString::number(mupdf::pdf::relativeHeight);
		} else {
			// Default
			mupdf::pdf::width_qstr = QString::number(mupdf::pdf::width);
			mupdf::pdf::height_qstr = QString::number(mupdf::pdf::height);
		}
		if (global::reader::globalReadingSettings == false) {
			if (goToSavedPageDone == false) {
				mupdf::pdf::pdfPageNumber = readFile(".config/A-page_number/config").toInt();
				goToSavedPageDone = true;
			}
		}
		if (mupdf::pdf::pdfPageNumber <= 0) {
			mupdf::pdf::pdfPageNumber = 1;
		}
		mupdf::pdf::pdfPageNumber_qstr = QString::number(mupdf::pdf::pdfPageNumber);
	} else if (fileType == 2) {
		;
	}
	if (mupdf::convertRelativeValues == true) {
		mupdf::convertRelativeValues = false;
	}
}

void ReaderWidget::setPageStyle(int fileType) {
	/* fileType can be:
	 * 0: ePUB
	 * 1: PDF
	 * 2: Image
	 */
	if (fileType == 0) {
		// General page size
		defineDefaultPageSize(0);

		if (readFile(".config/13-epub_page_size/width").isEmpty()) {
			QString pageWidth = QString::number(defaultEpubPageWidth);
			writeFile(".config/13-epub_page_size/width", pageWidth);
			writeFile(".config/13-epub_page_size/set", "true");
		}
		mupdf::epub::width = readFile(".config/13-epub_page_size/width").toInt();

		if (readFile(".config/13-epub_page_size/height").isEmpty()) {
			QString pageHeight = QString::number(defaultEpubPageHeight);
			writeFile(".config/13-epub_page_size/height", pageHeight);
			writeFile(".config/13-epub_page_size/set", "true");
		}
		mupdf::epub::height = readFile(".config/13-epub_page_size/height").toInt();
	} else if (fileType == 1) {
		defineDefaultPageSize(1);
	}
}

void ReaderWidget::delay(int mseconds) {
	// https://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
	QTime dieTime = QTime::currentTime().addMSecs(mseconds);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void ReaderWidget::on_text_selectionChanged() {
	delay(100);
	if (wordwidgetLock == false and textDialogLock == false) {
		QTextCursor cursor = ui->text->textCursor();
		selected_text = cursor.selectedText();
		if (!selected_text.isEmpty()) {
			log("Text selection changed; selected text: '" + selected_text + "'", className);
			if (!selected_text.contains(" ")) {
				// Word selection
				QString dictionary_position_str = QString::number(dictionary_position);
				ui->definitionStatusLabel->setText(dictionary_position_str);

				selected_text = selected_text.toLower();
				QStringList parts = selected_text.split(' ', QString::SkipEmptyParts);
				for (int i = 0; i < parts.size(); ++i)
					parts[i].replace(0, 1, parts[i][0].toUpper());
				word = parts.join(" ");
				letter = word.left(1);
				selected_text_str = word.toStdString();
				dictionary_lookup(selected_text_str, letter, dictionary_position);
				ui->wordLabel->setText(word);
				ui->definitionLabel->setText(definition);
				if (checkconfig_match(".config/06-words/config", selected_text_str) == true) {
					ui->saveWordBtn->setText("");
					ui->saveWordBtn->setIcon(QIcon(":/resources/starred_star.png"));
				} else {
					ui->saveWordBtn->setText("");
					ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
				}
				wordwidgetLock = true;
				wordwidget_show();
			} else {
				// Highlight
				textDialogLock = true;
				global::reader::highlightAlreadyDone = false;
				QJsonObject jsonObject = getHighlightsForBook(book_file);
				QString htmlText = ui->text->toHtml();
				if (htmlText.contains("<span style=\" font-size:" + QString::number(global::reader::fontSize) + "pt; background-color:#bbbbbb;\">" + selected_text + "</span>") or htmlText.contains("<span style=\" background-color:#bbbbbb;\">" + selected_text + "</span>")) {
					log("Highlight already done", className);
					global::reader::highlightAlreadyDone = true;
				}

				TextDialog *textDialogWindow = new TextDialog(this);
				QObject::connect(textDialogWindow, &TextDialog::destroyed, this, &ReaderWidget::unsetTextDialogLock);
				QObject::connect(textDialogWindow, &TextDialog::highlightText, this, &ReaderWidget::highlightText);
				QObject::connect(textDialogWindow, &TextDialog::unhighlightText, this, &ReaderWidget::unhighlightText);
				textDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
				textDialogWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
				textDialogWindow->move(mapFromGlobal(ui->text->cursorRect().bottomRight()));
				textDialogWindow->show();
			}
		} else {
			;
		}
	}
}

void ReaderWidget::on_nightModeBtn_clicked() {
	if (isNightModeActive == true) {
		// Disabling night/dark mode
		log("Setting night mode to OFF", className);
		writeFile("/tmp/invertScreen", "n");
		writeFile(".config/10-dark_mode/config", "false");
		ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-empty.png"));
		isNightModeActive = false;
	} else {
		// Enabling night/dark mode
		log("Setting night mode to ON", className);
		writeFile("/tmp/invertScreen", "y");
		writeFile(".config/10-dark_mode/config", "true");
		ui->nightModeBtn->setIcon(QIcon(":/resources/nightmode-full.png"));
		isNightModeActive = true;
	}
}

void ReaderWidget::openUsbmsDialog() {
	log("Showing USBMS dialog", className);
	global::usbms::showUsbmsDialog = false;
	global::usbms::usbmsDialog = true;

	generalDialogWindow = new GeneralDialog(this);
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	QApplication::processEvents();
}

QString ReaderWidget::setPageNumberLabelContent() {
	QString content;
	if (is_epub == true) {
		QString pageNumber;
		QString totalPages;
		pageNumberInt = mupdf::epub::epubPageNumber;
		pageNumber = QString::number(pageNumberInt);
		totalPages = QString::number(totalPagesInt);
		content.append(pageNumber);
		content.append(" <i>of</i> ");
		content.append(totalPages);
	} else if (is_pdf == true) {
		QString pageNumber;
		QString totalPages;
		pageNumberInt = mupdf::pdf::pdfPageNumber;
		pageNumber = QString::number(pageNumberInt);
		totalPages = QString::number(totalPagesInt);
		content.append(pageNumber);
		content.append(" <i>of</i> ");
		content.append(totalPages);
	} else {
		QString pageNumber;
		QString totalPages;
		pageNumberInt = split_files_number - split_total;
		totalPagesInt = split_files_number - 2;
		pageNumber = QString::number(pageNumberInt);
		totalPages = QString::number(totalPagesInt);
		content.append(pageNumber);
		content.append(" <i>of</i> ");
		content.append(totalPages);
	}
	return content;
}

void ReaderWidget::setupPageWidget() {
	QString pageNumberInfoLabelContent = setPageNumberLabelContent();
	ui->pageNumberLabel->setText(pageNumberInfoLabelContent);
	ui->pageProgressBar->setMaximum(totalPagesInt);
	ui->pageProgressBar->setMinimum(1);
	ui->pageProgressBar->setValue(pageNumberInt);
}

void ReaderWidget::getTotalEpubPagesNumber() {
	QString epubProg("sh");
	QStringList epubArgs;
	convertMuPdfVars(0, true);
	epubArgs << "/mnt/onboard/.adds/inkbox/epub.sh" << mupdf::epub::fontSize_qstr << mupdf::epub::width_qstr << mupdf::epub::height_qstr << mupdf::epub::epubPageNumber_qstr << "get_pages_number";
	QProcess *epubProc = new QProcess();
	epubProc->start(epubProg, epubArgs);
	epubProc->waitForFinished();
	epubProc->deleteLater();

	QString totalPages = readFile("/run/epub_total_pages_number");
	totalPagesInt = totalPages.toInt();
	log("ePUB total pages number: " + totalPages, className);
	QFile::remove("/run/epub_total_pages_number");
}

void ReaderWidget::on_gotoBtn_clicked() {
	log("Showing 'Go to page' dialog", className);
	global::keyboard::keypadDialog = true;
	generalDialogWindow = new GeneralDialog();
	generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	connect(generalDialogWindow, SIGNAL(gotoPageSelected(int)), SLOT(gotoPage(int)));
}

void ReaderWidget::gotoPage(int pageNumber) {
	log("Going to page " + QString::number(pageNumber), className);
	if (is_epub == true) {
		if (pageNumber > totalPagesInt or pageNumber < 1) {
			showToast("Request is beyond page range");
		} else {
			mupdf::epub::epubPageNumber = pageNumber;
			setup_book(book_file, mupdf::epub::epubPageNumber, true);
			ui->text->setText("");
			ui->text->setText(epubPageContent);
			global::reader::currentViewportText = ui->text->toHtml();

			pagesTurned = 0;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	} else if (is_pdf == true) {
		if (pageNumber > totalPagesInt or pageNumber < 1) {
			showToast("Request is beyond page range");
		} else {
			mupdf::pdf::pdfPageNumber = pageNumber;
			setup_book(book_file, mupdf::pdf::pdfPageNumber, true);
			setupPng();

			pagesTurned = 0;
			writeconfig_pagenumber(false);
		}
	} else {
		if (split_files_number - pageNumber < 2 or split_files_number - pageNumber > split_files_number - 1) {
			showToast("You've reached the end of the document");
		} else {
			split_total = split_files_number - pageNumber;

			setup_book(book_file, split_total, false);
			ui->text->setText("");
			ui->text->setText(ittext);

			pagesTurned = 0;
			writeconfig_pagenumber(false);
			setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
		}
	}
	setupPageWidget();
	refreshScreen();
}

void ReaderWidget::on_searchBtn_clicked() {
	global::forbidOpenSearchDialog = false;
	setupSearchDialog();
}

void ReaderWidget::setupSearchDialog() {
	if (global::forbidOpenSearchDialog == false) {
		log("Launching Search dialog", className);
		global::keyboard::keyboardDialog = true;
		global::keyboard::searchDialog = true;
		global::keyboard::keyboardText = "";
		generalDialogWindow = new GeneralDialog();
		generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
		connect(generalDialogWindow, SIGNAL(refreshScreen()), SLOT(searchRefreshScreen()));
		connect(generalDialogWindow, SIGNAL(destroyed(QObject *)), SLOT(setupSearchDialog()));
		connect(generalDialogWindow, SIGNAL(openBookFile(QString, bool)), SLOT(openBookFileNative(QString, bool)));
		connect(generalDialogWindow, SIGNAL(showToast(QString)), SLOT(showToast(QString)));
		connect(generalDialogWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToast()));
		generalDialogWindow->show();
	} else {
		;
	}
}

void ReaderWidget::searchRefreshScreen() {
	this->repaint();
}

void ReaderWidget::showToast(QString messageToDisplay) {
	global::toast::message = messageToDisplay;
	toastWindow = new toast(this);
	toastWindow->setAttribute(Qt::WA_DeleteOnClose);
	toastWindow->show();
}

void ReaderWidget::saveReadingSettings() {
	writeconfig_pagenumber(true);
}

void ReaderWidget::setupLocalSettingsEnvironment() {
	log("Setting local settings environment up", className);
	QString pageNumberDirPath = ".config/A-page_number";
	QDir pageNumberDir;
	pageNumberDir.mkpath(pageNumberDirPath);
}

void ReaderWidget::setupPng() {
	if (is_pdf == true) {
		// Note: Output file is supposed to be '/run/page.png', but somehow mutool puts it in '/run/page1.png'
		QPixmap pixmap("/run/page1.png");
		QFile::remove("/run/page1.png");
		// Initialized above
		graphicsScene->clear();
		graphicsScene->addPixmap(pixmap);
		// Shrinking scene if item is smaller than previous one
		QRectF rect = graphicsScene->itemsBoundingRect();
		graphicsScene->setSceneRect(rect);
		ui->graphicsView->items().clear();
		ui->graphicsView->setScene(graphicsScene);
		if (global::reader::pdfOrientation == 1) {
			if (!initialPdfRotationDone) {
				ui->graphicsView->rotate(270);
				initialPdfRotationDone = true;
			}
		}
	} else if (is_image == true) {
		QPixmap pixmap("/run/image.png");
		QFile::remove("/run/image.png");
		// Initialized above
		graphicsScene->clear();
		graphicsScene->addPixmap(pixmap);
		// Shrinking scene if item is smaller than previous one
		QRectF rect = graphicsScene->itemsBoundingRect();
		graphicsScene->setSceneRect(rect);
		ui->graphicsView->items().clear();
		ui->graphicsView->setScene(graphicsScene);
	}
}

bool ReaderWidget::pdf_file_match(QString file) {
	QString fileExt = file.right(3);

	if (fileExt == "pdf" or fileExt == "PDF") {
		log("Book file format: PDF", className);
		writeFile("/inkbox/bookIsPdf", "true");
		return true;
	} else {
		writeFile("/inkbox/bookIsPdf", "false");
		return false;
	}
}

bool ReaderWidget::image_file_match(QString file) {
	if (file.right(3) == "png" or file.right(3) == "PNG" or file.right(3) == "jpg" or file.right(3) == "JPG" or file.right(3) == "bmp" or file.right(3) == "BMP" or file.right(3) == "tif" or file.right(3) == "TIF") {
		log("File format: image", className);
		writeFile("/inkbox/bookIsImage", "true");
		return true;
	} else if (file.right(4) == "jpeg" or file.right(4) == "JPEG" or file.right(4) == "tiff" or file.right(4) == "TIFF") {
		log("File format: image", className);
		writeFile("/inkbox/bookIsImage", "true");
		return true;
	} else {
		writeFile("/inkbox/bookIsImage", "false");
		return false;
	}
}

void ReaderWidget::getTotalPdfPagesNumber() {
	QString epubProg("sh");
	QStringList epubArgs;
	convertMuPdfVars(0, true);
	epubArgs << "/mnt/onboard/.adds/inkbox/pdf_get_total_pages_number.sh" << book_file;
	QProcess *epubProc = new QProcess();
	epubProc->start(epubProg, epubArgs);
	epubProc->waitForFinished();
	epubProc->deleteLater();

	QString totalPages = readFile("/run/pdf_total_pages_number");
	totalPagesInt = totalPages.toInt();
	log("Total PDF pages number: " + totalPages, className);
	QFile::remove("/run/pdf_total_pages_number");
}

void ReaderWidget::on_pdfScaleSlider_valueChanged(int value) {
	log("Setting PDF scale to " + QString::number(value), className);
	if (value == 1) {
		mupdf::pdf::relativeWidth = 1 * mupdf::pdf::width;
		mupdf::pdf::relativeHeight = 1 * mupdf::pdf::height;
	} else if (value == 2) {
		mupdf::pdf::relativeWidth = 1.50 * mupdf::pdf::width;
		mupdf::pdf::relativeHeight = 1.50 * mupdf::pdf::height;
	} else if (value == 3) {
		mupdf::pdf::relativeWidth = 2 * mupdf::pdf::width;
		mupdf::pdf::relativeHeight = 2 * mupdf::pdf::height;
	} else if (value == 4) {
		mupdf::pdf::relativeWidth = 2.50 * mupdf::pdf::width;
		mupdf::pdf::relativeHeight = 2.50 * mupdf::pdf::height;
	}

	if (value != 1) {
		ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	} else {
		if (checkconfig(".config/14-reader_scrollbar/config") == true) {
			ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		} else {
			ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}

	mupdf::convertRelativeValues = true;
	setup_book(book_file, mupdf::pdf::pdfPageNumber, true);
	setupPng();
}

void ReaderWidget::on_decreaseScaleBtn_clicked() {
	int sliderCurrentValue;
	int sliderWantedValue;
	sliderCurrentValue = ui->pdfScaleSlider->value();
	sliderWantedValue = sliderCurrentValue - 1;
	if (sliderWantedValue < ui->pdfScaleSlider->QAbstractSlider::minimum()) {
		showToast("Minimum scale reached");
	} else {
		ui->pdfScaleSlider->setValue(sliderWantedValue);
	}
}

void ReaderWidget::on_increaseScaleBtn_clicked() {
	int sliderCurrentValue;
	int sliderWantedValue;
	sliderCurrentValue = ui->pdfScaleSlider->value();
	sliderWantedValue = sliderCurrentValue + 1;
	if (sliderWantedValue > ui->pdfScaleSlider->QAbstractSlider::maximum()) {
		showToast("Maximum scale reached");
	} else {
		ui->pdfScaleSlider->setValue(sliderWantedValue);
	}
}

void ReaderWidget::openBookFileNative(QString book, bool relativePath) {
	if (global::runningInstanceIsReaderOnly == false) {
		emit openBookFile(book, relativePath);
	} else {
		showToast("Not supported");
	}
}

void ReaderWidget::on_quitBtn_clicked() {
	log("Showing Quit window", className);
	writeconfig_pagenumber(true);
	quitWindow = new QuitWidget();
	quitWindow->setAttribute(Qt::WA_DeleteOnClose);
	quitWindow->showFullScreen();
}

void ReaderWidget::closeIndefiniteToast() {
	// Warning: use with caution
	toastWindow->close();
}

void ReaderWidget::getPdfOrientation(QString file) {
	log("Getting viewport orientation for PDF file '" + file + "'", className);
	writeFile("/inkbox/pdf_orientation_file_request", file);
	writeFile("/opt/ibxd", "get_pdf_orientation\n");
	while (true) {
		if (QFile::exists("/inkbox/pdf_orientation_result")) {
			QString result = readFile("/inkbox/pdf_orientation_result").trimmed();
			if (result == "Portrait") {
				QString function = __func__;
				log(function + ": Orientation is portrait", className);
				global::reader::pdfOrientation = 0;
			} else {
				QString function = __func__;
				log(function + ": Orientation is landscape", className);
				global::reader::pdfOrientation = 1;
			}
			break;
		}
	}
}

void ReaderWidget::unsetTextDialogLock() {
	QTextCursor cursor = ui->text->textCursor();
	cursor.clearSelection();
	ui->text->setTextCursor(cursor);

	textDialogLock = false;
}

void ReaderWidget::highlightText() {
	log("Highlighting text '" + selected_text + "'", className);
	highlightBookText(selected_text, book_file, false);
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
}

void ReaderWidget::unhighlightText() {
	log("Removing highlighted text '" + selected_text + "'", className);
	highlightBookText(selected_text, book_file, true);
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
}

void ReaderWidget::on_viewHighlightsBtn_clicked() {
	log("Launching highlights list dialog for book '" + book_file + "'", className);
	QJsonObject jsonObject = getHighlightsForBook(book_file);
	if (jsonObject.isEmpty() or jsonObject.length() <= 1) {
		global::toast::delay = 3000;
		showToast("No highlights for this book");
	} else {
		global::highlightsListDialog::bookPath = book_file;
		HighlightsListDialog *highlightsListDialogWindow = new HighlightsListDialog(this);
		QObject::connect(highlightsListDialogWindow, &HighlightsListDialog::destroyed, this, &ReaderWidget::setTextPropertiesSlot);
		QObject::connect(highlightsListDialogWindow, &HighlightsListDialog::showToast, this, &ReaderWidget::showToast);
		highlightsListDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	}
}

void ReaderWidget::setTextPropertiesSlot() {
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
}

void ReaderWidget::setInitialTextProperties() {
	// Checking saved font size if any, and if there is, set it
	QString fontSizeString = readFile(".config/04-book/size");
	if (!fontSizeString.isEmpty()) {
		global::reader::fontSize = fontSizeString.toInt();
		ui->sizeSlider->setValue(global::reader::fontSize);
	} else {
		global::reader::fontSize = 4;
	}
}

void ReaderWidget::setLineSpacing(int spacing, bool write) {
	global::reader::lineSpacing = spacing;
	if (write == true and readFile(".config/04-book/line_spacing") != QString::number(global::reader::lineSpacing)) {
		writeFile(".config/04-book/line_spacing", QString::number(global::reader::lineSpacing));
	}

	if (write == false) {
		log("Setting line spacing to " + QString::number(global::reader::lineSpacing + 1), className);
		QTextCursor textCursor = ui->text->textCursor();

		QTextBlockFormat *newFormat = new QTextBlockFormat();
		textCursor.clearSelection();
		textCursor.select(QTextCursor::Document);
		newFormat->setLineHeight((global::reader::lineSpacing * 10 + 100), QTextBlockFormat::ProportionalHeight);
		newFormat->setLeftMargin(global::reader::margins * 10);
		newFormat->setRightMargin(global::reader::margins * 10);
		textCursor.setBlockFormat(*newFormat);
	}
}

void ReaderWidget::setMargins(int margins, bool write) {
	global::reader::margins = margins;
	if (write == true and readFile(".config/04-book/margins") != QString::number(global::reader::margins)) {
		writeFile(".config/04-book/margins", QString::number(global::reader::margins));
	}

	if (write == false) {
		log("Setting margins to " + QString::number(global::reader::margins), className);
		QTextCursor textCursor = ui->text->textCursor();

		QTextBlockFormat *newFormat = new QTextBlockFormat();
		textCursor.clearSelection();
		textCursor.select(QTextCursor::Document);
		newFormat->setLineHeight((global::reader::lineSpacing * 10 + 100), QTextBlockFormat::ProportionalHeight);
		newFormat->setLeftMargin(global::reader::margins * 10);
		newFormat->setRightMargin(global::reader::margins * 10);
		textCursor.setBlockFormat(*newFormat);
	}
}

void ReaderWidget::on_lineSpacingSlider_valueChanged(int value) {
	global::reader::lineSpacing = value;
	// Write setting once
	setLineSpacing(global::reader::lineSpacing, true);
	// Be consistent with other settings
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	ui->lineSpacingValueLabel->setText(QString::number(value + 1));
}

void ReaderWidget::on_marginsSlider_valueChanged(int value) {
	global::reader::margins = value;
	// Write setting once
	setMargins(global::reader::margins, true);
	// Be consistent with other settings
	setTextProperties(global::reader::textAlignment, global::reader::lineSpacing, global::reader::margins, global::reader::font, global::reader::fontSize);
	ui->marginsValueLabel->setText(QString::number(value + 1));
}

void ReaderWidget::on_brightnessBtn_clicked() {
	log("Showing Brightness Dialog", className);
	brightnessDialogWindow = new BrightnessDialog();
	brightnessDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
	brightnessDialogWindow->show();
}

void ReaderWidget::setCinematicBrightnessWarmthSlot() {
	Device *device = Device::getSingleton();
	//FIXME: I have no clue what this is supposed to do, it sets the brightness value to itself?
	if (global::reader::globalReadingSettings == false) {
		if (device->supportsBrightness()) {
			int brightness_value = device->getBrightness();
			log("Local Reading Settings: Setting brightness to " + QString::number(brightness_value), className);
			UI::transitionBrightness(brightness_value);
		}
		if (device->supportsWarmLight()) {
			int warmthValue = device->getWarmth();
			log("Local Reading Settings: Setting warmth to " + QString::number(warmthValue), className);
            UI::transitionWarmth(warmthValue);
		}
	}
}
