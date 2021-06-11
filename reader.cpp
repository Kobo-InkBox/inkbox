#include "reader.h"
#include "ui_reader.h"
#include "functions.h"
#include <QProcess>
#include <QFileDialog>
#include <QDir>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QFont>
#include <QIcon>
#include <QSize>
#include <QDesktopWidget>
#include <QScreen>
#include <QFontDatabase>
#include <QDirIterator>
#include <QDebug>
#include <QTextDocument>
#include <QTextCodec>

using namespace std;

reader::reader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::reader)
{
    // Variables
    global::battery::showLowBatteryDialog = true;
    global::battery::showCriticalBatteryAlert = true;

    // QTextDocument
    QTextDocument *text = new QTextDocument();


    ui->setupUi(this);
    ui->previousBtn->setProperty("type", "borderless");
    ui->nextBtn->setProperty("type", "borderless");
    ui->hideOptionsBtn->setProperty("type", "borderless");
    ui->optionsBtn->setProperty("type", "borderless");
    ui->brightnessDecBtn->setProperty("type", "borderless");
    ui->brightnessIncBtn->setProperty("type", "borderless");
    ui->homeBtn->setProperty("type", "borderless");
    ui->aboutBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignRightBtn->setProperty("type", "borderless");
    ui->alignCenterBtn->setProperty("type", "borderless");
    ui->alignLeftBtn->setProperty("type", "borderless");
    ui->alignJustifyBtn->setProperty("type", "borderless");
    ui->infoCloseBtn->setProperty("type", "borderless");
    ui->saveWordBtn->setProperty("type", "borderless");
    ui->previousDefinitionBtn->setProperty("type", "borderless");
    ui->nextDefinitionBtn->setProperty("type", "borderless");

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
    ui->brightnessDecBtn->setText("");
    ui->brightnessDecBtn->setIcon(QIcon(":/resources/minus.png"));
    ui->brightnessIncBtn->setText("");
    ui->brightnessIncBtn->setIcon(QIcon(":/resources/plus.png"));

    // Making text selectable
    ui->text->setTextInteractionFlags(Qt::TextSelectableByMouse);

    // Custom settings
    // Font
    string_checkconfig(".config/04-book/font");
    if(checkconfig_str_val == "") {
        ui->fontChooser->setCurrentText(checkconfig_str_val);
        ui->text->setFont(QFont("Inter"));
    }
    else {
        if(checkconfig_str_val == "Crimson Pro") {
            int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Regular.ttf");
            QString family = QFontDatabase::applicationFontFamilies(id).at(0);
            QFont crimson(family);
            ui->text->setFont(crimson);
            ui->fontChooser->setCurrentText(checkconfig_str_val);
        }
        else {
            QFont config_font(checkconfig_str_val);
            ui->text->setFont(config_font);
            ui->fontChooser->setCurrentText(checkconfig_str_val);
        }
    }

    // Alignment
    string_checkconfig(".config/04-book/alignment");
    if (checkconfig_str_val == "") {
        ;
    }
    else {
        if(checkconfig_str_val == "Left") {
            ui->text->setAlignment(Qt::AlignLeft);
        }
        if(checkconfig_str_val == "Center") {
            ui->text->setAlignment(Qt::AlignHCenter);
        }
        if(checkconfig_str_val == "Right") {
            ui->text->setAlignment(Qt::AlignRight);
        }
        if(checkconfig_str_val == "Justify") {
            ui->text->setAlignment(Qt::AlignJustify);
        }
    }

    // Stylesheet + misc.
    QFile stylesheetFile(":/resources/eink.qss");
    stylesheetFile.open(QFile::ReadOnly);
    this->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();

    ui->sizeValueLabel->setStyleSheet("font-size: 9pt");
    ui->homeBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->aboutBtn->setStyleSheet("font-size: 9pt; padding: 5px");
    ui->fontChooser->setStyleSheet("font-size: 9pt");

    // Hiding the menubar + definition widget + brightness widget
    ui->hideOptionsBtn->hide();
    ui->menuWidget->setVisible(false);
    ui->brightnessWidget->setVisible(false);
    ui->menuBarWidget->setVisible(false);
    ui->wordWidget->setVisible(false);
    if(checkconfig(".config/11-menubar/sticky") == true) {
        ui->menuWidget->setVisible(true);
        ui->spacerWidget->setVisible(true);
        ui->statusBarWidget->setVisible(true);
    }
    else {
        ui->menuWidget->setVisible(false);
        ui->spacerWidget->setVisible(false);
        ui->statusBarWidget->setVisible(false);
    }

    // Getting brightness level
    int brightness_value = get_brightness();
    ui->brightnessStatus->setValue(brightness_value);

    // Defining pixmaps
    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    // Defining what the icons' size will be
    if(checkconfig("/opt/inkbox_genuine") == true) {
        string_checkconfig_ro("/opt/inkbox_device");
        if(checkconfig_str_val == "n705\n" or checkconfig_str_val == "n905\n") {
            float stdIconWidth = sW / 16;
            float stdIconHeight = sW / 16;
            QPixmap chargingPixmap(":/resources/battery_charging.png");
            scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap fullPixmap(":/resources/battery_full.png");
            scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap halfPixmap(":/resources/battery_half.png");
            scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap emptyPixmap(":/resources/battery_empty.png");
            scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        }
        else {
            float stdIconWidth = sW / 19;
            float stdIconHeight = sH / 19;
            QPixmap chargingPixmap(":/resources/battery_charging.png");
            scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap fullPixmap(":/resources/battery_full.png");
            scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap halfPixmap(":/resources/battery_half.png");
            scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
            QPixmap emptyPixmap(":/resources/battery_empty.png");
            scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        }
    }
    else {
        float stdIconWidth = sW / 19;
        float stdIconHeight = sH / 19;
        QPixmap chargingPixmap(":/resources/battery_charging.png");
        scaledChargingPixmap = chargingPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap fullPixmap(":/resources/battery_full.png");
        scaledFullPixmap = fullPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap halfPixmap(":/resources/battery_half.png");
        scaledHalfPixmap = halfPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
        QPixmap emptyPixmap(":/resources/battery_empty.png");
        scaledEmptyPixmap = emptyPixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio);
    }

    // Checking if there is a page refresh setting set
    string_checkconfig(".config/04-book/refresh");
    if(checkconfig_str_val == "") {
        // Writing the default, refresh every 3 pages
        string_writeconfig(".config/04-book/refresh", "3");
        string_checkconfig(".config/04-book/refresh");
    }
    else {
        // A config option was set, continuing after the Else statement...
        ;
    }
    pageRefreshSetting = checkconfig_str_val.toInt();
    // Checking if that config option was set to "Never refresh"...
    if(pageRefreshSetting == -1) {
        neverRefresh = true;
    }
    else {
        // Safety measure
        neverRefresh = false;
    }

    // Clock setting to show seconds + battery level
    if(checkconfig(".config/02-clock/config") == true) {
        QTimer *t = new QTimer(this);
        t->setInterval(500);
        connect(t, &QTimer::timeout, [&]() {
           QString time = QTime::currentTime().toString("hh:mm:ss");
           get_battery_level();
           ui->batteryLabel->setText(batt_level);
           ui->timeLabel->setText(time);
        } );
        t->start();
    }
    else {
        QTimer *t = new QTimer(this);
        t->setInterval(500);
        connect(t, &QTimer::timeout, [&]() {
           QString time = QTime::currentTime().toString("hh:mm");
           get_battery_level();
           ui->batteryLabel->setText(batt_level);
           ui->timeLabel->setText(time);
        } );
        t->start();
    }

    // Word selection & dictionary lookup feature
    /*QString dictionary_position_str = QString::number(dictionary_position);
    ui->definitionStatusLabel->setText(dictionary_position_str);
    QTimer *select_t = new QTimer(this);
    select_t->setInterval(100);
    connect(select_t, &QTimer::timeout, [&]() {
        selected_text = ui->text->selectedText();
        if(ui->text->hasSelectedText() == true) {
            if(selected_text_lock == false) {
                selected_text_lock = true;
                QStringList parts = selected_text.split(' ', QString::SkipEmptyParts);
                for (int i = 0; i < parts.size(); ++i)
                    parts[i].replace(0, 1, parts[i][0].toUpper());
                word = parts.join(" ");
                letter = word.left(1);
                selected_text_str = word.toStdString();
                dictionary_lookup(selected_text_str, letter, dictionary_position);
                ui->wordLabel->setText(word);
                ui->definitionLabel->setText(definition);
                if(checkconfig_match(".config/06-words/config", selected_text_str) == true) {
                    ui->saveWordBtn->setText("");
                    ui->saveWordBtn->setIcon(QIcon(":/resources/starred_star.png"));
                }
                else {
                    ui->saveWordBtn->setText("");
                    ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
                }
                wordwidget_show();
            }
            else {
                ;
            }
        }
        else {
            ;
        }
    } );
    select_t->start();*/

    // We have to get the file's path
    if(global::reader::skipOpenDialog == true) {
        if(checkconfig("/tmp/suspendBook") == true) {
            wakeFromSleep = true;
            // Prevent from opening the Reader framework next time unless the condition is reset
            string_writeconfig("/tmp/suspendBook", "false");
            book_file = "/inkbox/book/book.txt";
        }
        else {
            if(global::reader::bookNumber == 1) {
                string_checkconfig(".config/08-recent_books/1");
                book_file = checkconfig_str_val;
            }
            if(global::reader::bookNumber == 2) {
                string_checkconfig(".config/08-recent_books/2");
                book_file = checkconfig_str_val;
            }
            if(global::reader::bookNumber == 3) {
                string_checkconfig(".config/08-recent_books/3");
                book_file = checkconfig_str_val;
            }
            if(global::reader::bookNumber == 4) {
                string_checkconfig(".config/08-recent_books/4");
                book_file = checkconfig_str_val;
            }
        }
    }
    else {
        if(checkconfig("/opt/inkbox_genuine") == true) {
            QDir::setCurrent("/mnt/onboard/onboard");
            QFileDialog *dialog = new QFileDialog(this);
            // https://forum.qt.io/topic/29471/solve-how-to-show-qfiledialog-at-center-position-screen/4
            QDesktopWidget desk;
            QRect screenres = desk.screenGeometry(0); dialog->setGeometry(QRect(screenres.width()/4,screenres.height() /4,screenres.width()/2,screenres.height()/2));
            stylesheetFile.open(QFile::ReadOnly);
            dialog->setStyleSheet(stylesheetFile.readAll());
            stylesheetFile.close();
            book_file = dialog->getOpenFileName(dialog, tr("Open File"), QDir::currentPath());

            if(book_file != "") {
                QDir::setCurrent("/mnt/onboard/.adds/inkbox");
            }
            else {
                // User clicked "Cancel" button
                QDir::setCurrent("/mnt/onboard/.adds/inkbox");
                quit_restart();
            }
        }
        else {
            QDir::setCurrent("/mnt/onboard");
            QFileDialog *dialog = new QFileDialog(this);
            // https://forum.qt.io/topic/29471/solve-how-to-show-qfiledialog-at-center-position-screen/4
            QDesktopWidget desk;
            QRect screenres = desk.screenGeometry(0); dialog->setGeometry(QRect(screenres.width()/4,screenres.height() /4,screenres.width()/2,screenres.height()/2));
            stylesheetFile.open(QFile::ReadOnly);
            dialog->setStyleSheet(stylesheetFile.readAll());
            stylesheetFile.close();
            book_file = dialog->getOpenFileName(dialog, tr("Open File"), QDir::currentPath());

            if(book_file != "") {
                QDir::setCurrent("/mnt/onboard/.adds/inkbox");
            }
            else {
                // User clicked "Cancel" button
                QDir::setCurrent("/mnt/onboard/.adds/inkbox");
                quit_restart();
            }
        }
    }

    // Checking if we're waking from sleep; if so, do nothing there because the book should have already been parsed
    if(wakeFromSleep != true) {
        // Counting number of parsed files
        split_total = setup_book(book_file);
        split_files_number = split_total;
        split_total = split_total - 1;

        writeconfig_pagenumber();
    }
    else {
        // Retrieve split_total from tmpfs
        string_checkconfig("/tmp/inkboxPageNumber");
        split_total = checkconfig_str_val.toInt();
        setup_book(book_file);
    }

    // Get text
    QDir::setCurrent("/mnt/onboard/.adds/inkbox");
    setup_book(book_file);

    // Display text
    // Checking saved font size if any
    string_checkconfig(".config/04-book/size");
    if(checkconfig_str_val == "0") {
        checkconfig_str_val = "6";
        ui->sizeSlider->setValue(0);
        ui->sizeValueLabel->setText("1");
    }
    if(checkconfig_str_val == "1") {
        checkconfig_str_val = "10";
        ui->sizeSlider->setValue(1);
        ui->sizeValueLabel->setText("2");
    }
    if(checkconfig_str_val == "2") {
        checkconfig_str_val = "14";
        ui->sizeSlider->setValue(2);
        ui->sizeValueLabel->setText("3");
    }
    if(checkconfig_str_val == "3") {
        checkconfig_str_val = "18";
        ui->sizeSlider->setValue(3);
        ui->sizeValueLabel->setText("4");
    }
    if(checkconfig_str_val == "4") {
        checkconfig_str_val = "22";
        ui->sizeSlider->setValue(4);
        ui->sizeValueLabel->setText("5");
    }
    if(checkconfig_str_val == "") {
        checkconfig_str_val = "10";
        ui->sizeSlider->setValue(1);
        ui->sizeValueLabel->setText("2");
    }
    QString font_size = "font-size: ";
    font_size = font_size.append(checkconfig_str_val);
    font_size = font_size.append("pt");
    ui->text->setStyleSheet(font_size);

    // Wheeee!
    ui->text->setText(ittext);

    // Clean up
    string_writeconfig("/inkbox/remount", "true");

    // Way to tell shell scripts that we're in the Reader framework
    string_writeconfig("/tmp/inkboxReading", "true");

    // Saving the book opened in the favorites list
    string_checkconfig(".config/08-recent_books/1");
    book_1 = checkconfig_str_val;
    string str_book_1 = book_1.toStdString();
    string_checkconfig(".config/08-recent_books/2");
    book_2 = checkconfig_str_val;
    string str_book_2 = book_2.toStdString();
    string_checkconfig(".config/08-recent_books/3");
    book_3 = checkconfig_str_val;
    string str_book_3 = book_3.toStdString();
    string_checkconfig(".config/08-recent_books/4");
    book_4 = checkconfig_str_val;
    string str_book_4 = book_4.toStdString();

    string book_file_str;

    // Don't mess up "Recently read books" with random "book.txt" buttons...
    if(wakeFromSleep == true) {
        string_checkconfig("/tmp/inkboxBookPath");
        book_file_str = checkconfig_str_val.toStdString();
    }
    else {
        book_file_str = book_file.toStdString();
        string_writeconfig("/tmp/inkboxBookPath", book_file_str);
    }

    if(book_1 == book_file) {
        ;
    }
    else {
        // Moves old items to the right and puts the new one at the left side
        string_writeconfig(".config/08-recent_books/1", book_file_str);
        string_writeconfig(".config/08-recent_books/2", str_book_1);
        string_writeconfig(".config/08-recent_books/3", str_book_2);
        string_writeconfig(".config/08-recent_books/4", str_book_3);
    }

    // Battery watchdog
    if(global::reader::startBatteryWatchdog == true) {
        QTimer *t = new QTimer(this);
        t->setInterval(2000);
        connect(t, &QTimer::timeout, [&]() {
            // Checking if battery level is low
            if(global::battery::showCriticalBatteryAlert != true) {
                ;
            }
            else {
                if(isBatteryCritical() == true) {
                    qDebug() << "Warning! Battery is at a critical charge level!";
                    openCriticalBatteryAlertWindow();
                }
            }

            if(global::battery::showLowBatteryDialog != true) {
                // Do nothing, since a dialog should already have been displayed and (probably) dismissed
                ;
            }
            else {
                if(isBatteryLow() == true) {
                    if(global::battery::batteryAlertLock == true) {
                        ;
                    }
                    else {
                        qDebug() << "Warning! Battery is low!";
                        string_checkconfig_ro("/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/status");
                        if(checkconfig_str_val == "Charging\n") {
                            ;
                        }
                        else {
                            openLowBatteryDialog();
                        }
                    }
                }
            }
        } );
        t->start();
    }
}

reader::~reader()
{
    delete ui;
}

int reader::setup_book(QString book) {
    QFile bookFile = book;
    bookFile.open(QIODevice::ReadOnly);
    content = bookFile.readAll();
    bookFile.close();

    ittext = content;
}

void reader::checkwords() {
    QFile words_list(".config/06-words/config");
    words_list.open(QIODevice::ReadWrite);
    QTextStream in (&words_list);
    words = in.readAll();
    words_list.close();
}

bool reader::epub_file_match(QString file) {
    QString fileExt = file.right(4);

    if(fileExt == "epub" or fileExt == "EPUB") {
        return true;
    }
    else {
        return false;
    }
}

void reader::dictionary_lookup(string word, QString first_letter, int position) {
    ofstream fhandler;
    fhandler.open("/inkbox/dictionary/word");
    fhandler << word;
    fhandler.close();

    QDir::setCurrent("dictionary");
    QDir::setCurrent(first_letter);
    QString lookup_prog ("sh");
    QStringList lookup_args;
    QString position_str = QString::number(position);
    lookup_args << "../scripts/lookup.sh" << position_str;
    QProcess *lookup_proc = new QProcess();
    lookup_proc->start(lookup_prog, lookup_args);
    lookup_proc->waitForFinished();

    QFile definition_file("/inkbox/dictionary/definition");
    definition_file.open(QIODevice::ReadWrite);
    QTextStream in (&definition_file);
    definition = in.readAll();
    definition = definition.remove(QRegExp("[\n]"));
    if(definition == "No definition found.") {
        nextdefinition_lock = true;
    }
    else {
        nextdefinition_lock = false;
    }
    definition_file.close();

    QDir::setCurrent("/mnt/onboard/.adds/inkbox");
}

void reader::save_word(string word, bool remove) {
    if(remove == false) {
        QFile words(".config/06-words/config");
        words.open(QIODevice::ReadWrite);
        QTextStream in (&words);
        QString words_list = in.readAll();
        string words_list_str = words_list.toStdString();
        words.close();

        ofstream fhandler;
        fhandler.open(".config/06-words/config");
        fhandler << words_list_str << word << "\n";
        fhandler.close();
    }
    else {
        ofstream fhandler;
        fhandler.open(".config/06-words/config");
        fhandler << word;
        fhandler.close();
    }
}

void reader::on_nextBtn_clicked()
{
    if(split_total - 1 == 1 or split_total - 1 == 0) {
        QMessageBox::critical(this, tr("Invalid argument"), tr("You've reached the end of the document."));
    }
    else {
        parser_ran = true;
        split_total = split_total - 1;

        setup_book(book_file);
        ui->text->setText("");
        ui->text->setText(ittext);

        pagesTurned = pagesTurned + 1;
        if(neverRefresh == true) {
            // Do nothing; "Never refresh" was set
            ;
        }
        else {
            if(pagesTurned >= pageRefreshSetting) {
                // Refreshing the screen
                this->repaint();
                // Reset count
                pagesTurned = 0;
            }
        }
        writeconfig_pagenumber();
    }
}

void reader::on_previousBtn_clicked()
{
    // Making sure we won't encounter a "List index out of range" error ;)
    if(split_total >= split_files_number - 1) {
        QMessageBox::critical(this, tr("Invalid argument"), tr("No previous page."));
    }
    else {
        parser_ran = true;
        split_total = split_total + 1;
        setup_book(book_file);
        ui->text->setText("");
        ui->text->setText(ittext);

        // We always increment pagesTurned regardless if we press the Previous or Next button
        pagesTurned = pagesTurned + 1;
        if(neverRefresh == true) {
            // Do nothing; "Never refresh" was set
            ;
        }
        else {
            if(pagesTurned >= pageRefreshSetting) {
                // Refreshing the screen
                this->repaint();
                // Reset count
                pagesTurned = 0;
            }
        }
        writeconfig_pagenumber();
    }
}

void reader::on_optionsBtn_clicked()
{
    menubar_show();
    this->repaint();
}

void reader::on_hideOptionsBtn_clicked()
{
    menubar_hide();
    this->repaint();
}

void reader::on_brightnessDecBtn_clicked()
{
    int bval = get_brightness();
    int set_bval = bval - 1;
    set_brightness(set_bval);
    brightness_writeconfig(set_bval);

    bval = get_brightness();
    ui->brightnessStatus->setValue(bval);
}

void reader::on_brightnessIncBtn_clicked()
{
    int bval = get_brightness();
    int set_bval = bval + 1;
    set_brightness(set_bval);
    brightness_writeconfig(set_bval);

    bval = get_brightness();
    ui->brightnessStatus->setValue(bval);
}

void reader::on_aboutBtn_clicked()
{
    if(checkconfig("/opt/inkbox_genuine") == true) {
        QString aboutmsg = "InkBox is an open-source, Qt-based eBook reader. It aims to bring you the latest Qt features while being also fast and responsive.";
        string_checkconfig_ro("/external_root/opt/isa/version");
        aboutmsg.append("\n\nInkBox ");
        aboutmsg.append(checkconfig_str_val);
        QMessageBox::information(this, tr("Information"), aboutmsg);
    }
    else {
        QMessageBox::information(this, tr("About"), tr("InkBox is an open-source Qt-based eBook reader. It aims to bring you the latest Qt features while being also fast and responsive."));
    }
}

void reader::on_homeBtn_clicked()
{
    // We're leaving reading mode
    string_writeconfig("/tmp/inkboxReading", "false");

    // Relaunching process
    quit_restart();
}

void reader::on_fontChooser_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "Roboto") {
        QFont roboto("Roboto");
        ui->text->setFont(roboto);
        string_writeconfig(".config/04-book/font", "Roboto");
    }
    if(arg1 == "Inter") {
        QFont inter("Inter");
        ui->text->setFont(inter);
        string_writeconfig(".config/04-book/font", "Inter");
    }
    if(arg1 == "Source Serif Pro") {
        QFont sourceserif("Source Serif Pro");
        ui->text->setFont(sourceserif);
        string_writeconfig(".config/04-book/font", "Source Serif Pro");
    }
    if(arg1 == "Libre Baskerville") {
        QFont librebaskerville("Libre Baskerville");
        ui->text->setFont(librebaskerville);
        string_writeconfig(".config/04-book/font", "Libre Baskerville");
    }
    if(arg1 == "Noto Mono") {
        QFont notomono("Noto Mono");
        ui->text->setFont(notomono);
        string_writeconfig(".config/04-book/font", "Noto Mono");
    }
    if(arg1 == "Libertinus Serif") {
        QFont libertinus("Libertinus Serif");
        ui->text->setFont(libertinus);
        string_writeconfig(".config/04-book/font", "Libertinus Serif");
    }
    if(arg1 == "Crimson Pro") {
        // As adding Crimson Pro to the default fonts bundled along with the Qt libs breaks the general Inter homogeneity, it is incorporated on-demand here.
        int id = QFontDatabase::addApplicationFont(":/resources/fonts/CrimsonPro-Regular.ttf");
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont crimson(family);
        ui->text->setFont(crimson);
        string_writeconfig(".config/04-book/font", "Crimson Pro");
    }
}

void reader::on_alignLeftBtn_clicked()
{
    ui->text->setAlignment(Qt::AlignLeft);
    string_writeconfig(".config/04-book/alignment", "Left");
}

void reader::on_alignCenterBtn_clicked()
{
    ui->text->setAlignment(Qt::AlignHCenter);
    string_writeconfig(".config/04-book/alignment", "Center");
}

void reader::on_alignRightBtn_clicked()
{
    ui->text->setAlignment(Qt::AlignRight);
    string_writeconfig(".config/04-book/alignment", "Right");
}

void reader::on_alignJustifyBtn_clicked()
{
    ui->text->setAlignment(Qt::AlignJustify);
    string_writeconfig(".config/04-book/alignment", "Justify");
}

void reader::menubar_show() {
    // Checking battery level and status, then displaying the relevant icon on batteryIconLabel
    string_checkconfig_ro("/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/status");
    if(checkconfig_str_val == "Charging\n") {
        ui->batteryIconLabel->setPixmap(scaledChargingPixmap);
    }
    else {
        get_battery_level();
        if(batt_level_int >= 75 && batt_level_int <= 100) {
            ui->batteryIconLabel->setPixmap(scaledFullPixmap);
        }
        if(batt_level_int >= 25 && batt_level_int <= 74) {
            ui->batteryIconLabel->setPixmap(scaledHalfPixmap);
        }
        if(batt_level_int >= 0 && batt_level_int <= 24) {
            ui->batteryIconLabel->setPixmap(scaledEmptyPixmap);
        }
    }

    ui->hideOptionsBtn->show();
    ui->optionsBtn->hide();
    ui->menuWidget->setVisible(true);
    ui->menuBarWidget->setVisible(true);
    ui->statusBarWidget->setVisible(true);

    string_checkconfig_ro("/opt/inkbox_device");
    if(checkconfig_str_val == "n705\n" or checkconfig_str_val == "n905\n") {
        ;
    }
    else {
        ui->brightnessWidget->setVisible(true);
    }
    if(checkconfig(".config/11-menubar/sticky") == true) {
        ui->spacerWidget->setVisible(false);
    }
    else {
        // Safety measure
        ui->spacerWidget->setVisible(false);
    }

    menubar_shown = true;
}

void reader::menubar_hide() {
    string_checkconfig_ro("/opt/inkbox_device");
    if(checkconfig_str_val == "n705\n" or checkconfig_str_val == "n905\n") {
        ui->brightnessWidget->setVisible(false);
    }
    else {
        // Safety measure
        ui->brightnessWidget->setVisible(false);
    }
    ui->hideOptionsBtn->hide();
    ui->optionsBtn->show();
    ui->menuBarWidget->setVisible(false);
    if(checkconfig(".config/11-menubar/sticky") == true) {
        ui->spacerWidget->setVisible(true);
        ui->statusBarWidget->setVisible(true);
    }
    else {
        ui->spacerWidget->setVisible(false);
        ui->statusBarWidget->setVisible(false);
        ui->menuWidget->setVisible(false);
    }
    menubar_shown = false;
}

void reader::wordwidget_show() {
    if(menubar_shown == true) {
        menubar_hide();
        ui->hideOptionsBtn->hide();
        ui->optionsBtn->hide();
        ui->line->hide();
        ui->wordWidget->setVisible(true);
    }
    else {
        ui->hideOptionsBtn->hide();
        ui->optionsBtn->hide();
        ui->line->hide();
        ui->wordWidget->setVisible(true);
    }
}

void reader::wordwidget_hide() {
    ui->wordWidget->setVisible(false);
    ui->hideOptionsBtn->hide();
    ui->optionsBtn->show();
    ui->line->show();
    selected_text_lock = false;
}

void reader::on_infoCloseBtn_clicked()
{
    wordwidget_hide();
    dictionary_position = 1;
    QString dictionary_position_str = QString::number(dictionary_position);
    ui->definitionStatusLabel->setText(dictionary_position_str);
}

void reader::on_previousDefinitionBtn_clicked()
{
    dictionary_position = dictionary_position - 1;
    if(dictionary_position <= 0) {
        dictionary_position = 1;
    }
    else {
        dictionary_lookup(selected_text_str, letter, dictionary_position);
        ui->definitionLabel->setText(definition);
        QString dictionary_position_str = QString::number(dictionary_position);
        ui->definitionStatusLabel->setText(dictionary_position_str);
    }
}

void reader::on_nextDefinitionBtn_clicked()
{
    dictionary_position = dictionary_position + 1;
    dictionary_lookup(selected_text_str, letter, dictionary_position);
    if(nextdefinition_lock == true) {
        dictionary_position = dictionary_position - 1;
    }
    else {
        ui->definitionLabel->setText(definition);
        QString dictionary_position_str = QString::number(dictionary_position);
        ui->definitionStatusLabel->setText(dictionary_position_str);
    }
}

void reader::on_saveWordBtn_clicked()
{
    if(checkconfig_match(".config/06-words/config", selected_text_str) == true) {
        checkwords();
        word = word.append("\n");
        words = words.replace(word, "");
        string words_std_string = words.toStdString();
        save_word(words_std_string, true);
        ui->saveWordBtn->setText("");
        ui->saveWordBtn->setIcon(QIcon(":/resources/star.png"));
    }
    else {
        save_word(selected_text_str, false);
        ui->saveWordBtn->setText("");
        ui->saveWordBtn->setIcon(QIcon(":/resources/starred_star.png"));
    }
}

void reader::on_sizeSlider_valueChanged(int value)
{
    // Font size
    string value_str = to_string(value);
    string_writeconfig(".config/04-book/size", value_str);

    // Future improvement?
    if(checkconfig_match("/opt/inkbox_device", "n705\n") == true) {
        if(value == 0) {
            ui->text->setStyleSheet("font-size: 6pt");
            ui->sizeValueLabel->setText("1");
        }
        if(value == 1) {
            ui->text->setStyleSheet("font-size: 10pt");
            ui->sizeValueLabel->setText("2");
        }
        if(value == 2) {
            ui->text->setStyleSheet("font-size: 14pt");
            ui->sizeValueLabel->setText("3");
        }
        if(value == 3) {
            ui->text->setStyleSheet("font-size: 18pt");
            ui->sizeValueLabel->setText("4");
        }
        if(value == 4) {
            ui->text->setStyleSheet("font-size: 22pt");
            ui->sizeValueLabel->setText("5");
        }
    }
    if(checkconfig_match("/opt/inkbox_device", "n905\n") == true) {
        if(value == 0) {
            ui->text->setStyleSheet("font-size: 6pt");
            ui->sizeValueLabel->setText("1");
        }
        if(value == 1) {
            ui->text->setStyleSheet("font-size: 10pt");
            ui->sizeValueLabel->setText("2");
        }
        if(value == 2) {
            ui->text->setStyleSheet("font-size: 14pt");
            ui->sizeValueLabel->setText("3");
        }
        if(value == 3) {
            ui->text->setStyleSheet("font-size: 18pt");
            ui->sizeValueLabel->setText("4");
        }
        if(value == 4) {
            ui->text->setStyleSheet("font-size: 22pt");
            ui->sizeValueLabel->setText("5");
        }
    }
    else {
        if(value == 0) {
            ui->text->setStyleSheet("font-size: 6pt");
            ui->sizeValueLabel->setText("1");
        }
        if(value == 1) {
            ui->text->setStyleSheet("font-size: 10pt");
            ui->sizeValueLabel->setText("2");
        }
        if(value == 2) {
            ui->text->setStyleSheet("font-size: 14pt");
            ui->sizeValueLabel->setText("3");
        }
        if(value == 3) {
            ui->text->setStyleSheet("font-size: 18pt");
            ui->sizeValueLabel->setText("4");
        }
        if(value == 4) {
            ui->text->setStyleSheet("font-size: 22pt");
            ui->sizeValueLabel->setText("5");
        }
    }
}

void reader::writeconfig_pagenumber() {
    // Saving the page number in tmpfs
    string split_total_str = to_string(split_total);
    string_writeconfig("/tmp/inkboxPageNumber", split_total_str);
}

void reader::quit_restart() {
    // Restarting InkBox
    QProcess process;
    process.startDetached("inkbox", QStringList());
    qApp->quit();
}

void reader::openLowBatteryDialog() {
    global::mainwindow::lowBatteryDialog = true;
    global::battery::batteryAlertLock = true;

    generalDialogWindow = new generalDialog(this);
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    generalDialogWindow->show();
    QApplication::processEvents();
}

void reader::openCriticalBatteryAlertWindow() {
    global::battery::showCriticalBatteryAlert = true;
    global::battery::showLowBatteryDialog = false;

    alertWindow = new alert();
    alertWindow->setAttribute(Qt::WA_DeleteOnClose);
    alertWindow->setGeometry(QRect(QPoint(0,0), screen()->geometry ().size()));
    alertWindow->show();
}
