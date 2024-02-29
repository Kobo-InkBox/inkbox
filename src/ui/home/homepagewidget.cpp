#include "homepagewidget.h"
#include "ui_homepagewidget.h"
#include "device.h"

#include <QScreen>

#include "functions.h"

HomePageWidget::HomePageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePageWidget)
{
    Device * device = Device::getSingleton();
    Device::ScreenSize screenSize = device->getScreenSize();

    ui->setupUi(this);

    // Getting the screen's size
    sW = QGuiApplication::screens()[0]->size().width();
    sH = QGuiApplication::screens()[0]->size().height();


    if(screenSize == Device::ScreenSize::SMALL) {
        stdIconWidth = sW / 23;
        stdIconHeight = sH / 23;
        bookTitleTruncateThreshold = 20;
        stdIconWidthDivider = 8;
        stdIconHeightDivider = 8;
    } else if(screenSize == Device::ScreenSize::MEDIUM) {
        stdIconWidth = sW / 23.5;
        stdIconHeight = sH / 23.5;
        bookTitleTruncateThreshold = 25;
        stdIconWidthDivider = 7;
        stdIconHeightDivider = 7;
    } else if(screenSize == Device::ScreenSize::LARGE) {
        stdIconWidth = sW / 24;
        stdIconHeight = sH / 24;
        bookTitleTruncateThreshold = 25;
        stdIconWidthDivider = 6.5;
        stdIconHeightDivider = 6.5;
    } else if(screenSize == Device::ScreenSize::VERY_LARGE) {
        stdIconWidth = sW / 25;
        stdIconHeight = sH / 25;
        bookTitleTruncateThreshold = 35;
        stdIconWidthDivider = 6;
        stdIconHeightDivider = 6;
    }


    ui->recentBooksIconLabel->setPixmap(QPixmap(":/resources/file-text.png").scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->pinnedBooksIconLabel->setPixmap(QPixmap(":/resources/pin.png").scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    horizontalLayoutArray.resize(global::homePageWidget::recentBooksRowNumber);
    verticalLayoutArray.resize(global::homePageWidget::recentBooksNumber);
    bookBtnArray.resize(global::homePageWidget::recentBooksNumber);
    bookTitleArray.resize(global::homePageWidget::recentBooksNumber);
    pinnedBooksHorizontalLayoutArray.resize(global::homePageWidget::pinnedBooksRowNumber);
    pinnedBooksVerticalLayoutArray.resize(global::homePageWidget::pinnedBooksNumber);
    pinnedBooksBtnArray.resize(global::homePageWidget::pinnedBooksNumber);
    pinnedBooksTitleArray.resize(global::homePageWidget::pinnedBooksNumber);

    stdIconWidth = sW / stdIconWidthDivider;
    stdIconHeight = sH / stdIconHeightDivider;

    if(!QFile::exists(global::localLibrary::databasePath)) {
        global::toast::modalToast = true;
        global::toast::indefiniteToast = true;
        showToast("Generating database");
        QTimer::singleShot(100, this, SLOT(setupDisplayWithDatabase()));
    }
    else {
        setupDisplay(false);
    }
}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::openBook(QString bookPath) {
    if(!bookPath.isEmpty()) {
        emit openBookSignal(bookPath, false);
    }
    else {
        showToast("Book not found");
    }
}

void HomePageWidget::refreshScreenNative() {
    emit refreshScreen();
}

void HomePageWidget::setupDisplay(bool databaseGenerated) {
    if(databaseGenerated == true) {
        toastWindow->close();
    }

    log("Reading database", className);
    QFile database(global::localLibrary::databasePath);
    QByteArray data;
    if(database.open(QIODevice::ReadOnly)) {
        data = database.readAll();
        database.close();
    }
    else {
        QString function = __func__; log(function + ": Failed to open local library database file for reading at '" + database.fileName() + "'", className);
    }

    /* Recent books */
    log("Reading recent books database", className);
    QFile recentBooksDatabase(global::localLibrary::recentBooksDatabasePath);
    QByteArray recentBooksData;
    if(recentBooksDatabase.open(QIODevice::ReadOnly)) {
        recentBooksData = recentBooksDatabase.readAll();
        recentBooksDatabase.close();
    }
    else {
        QString function = __func__; log(function + ": Failed to open recent books database file for reading at '" + recentBooksDatabase.fileName() + "'", className);
    }

    QJsonObject databaseJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(data))).object();
    QJsonArray databaseJsonArrayList = databaseJsonObject["database"].toArray();
    int databaseBooksNumber = databaseJsonArrayList.size();
    QJsonObject recentBooksJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(recentBooksData))).object();

    log("Setting up home page", className);

    if(!recentBooksJsonObject["Book1"].toObject().value("BookPath").toString().isEmpty()) {
        int in = 0;
        bool newRow = false;
        for(int i = 1; i <= (global::homePageWidget::recentBooksNumber + 1); i++) {
            if(in < global::homePageWidget::recentBooksRowNumber) {
                QString objectName = "Book" + QString::number(i);
                QJsonObject jsonObject = recentBooksJsonObject[objectName].toObject();
                QString bookPath = jsonObject.value("BookPath").toString();
                bookBtnArray[i] = new QClickableLabel(this);
                bookTitleArray[i] = new QToolTipLabel(this);

                // Iterate until we find a book matching the recently opened book's "BookPath" key/value pair
                for(int in = 1; in <= databaseBooksNumber; in++) {
                    QJsonObject bookJsonObject = databaseJsonArrayList.at(in - 1).toObject();
                    if(bookJsonObject["BookPath"].toString() == bookPath) {
                        QByteArray data = qCompress(QJsonDocument(bookJsonObject).toJson()).toBase64();
                        QString dataString = QString(data);
                        bookBtnArray[i]->setObjectName(dataString);
                    }
                }
                QJsonObject uncompressedJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(bookBtnArray[i]->objectName().toUtf8()))).object();

                verticalLayoutArray[i] = new QVBoxLayout();

                // Book icon button
                QObject::connect(bookBtnArray[i], &QClickableLabel::bookPath, this, &HomePageWidget::openBook);
                QObject::connect(bookBtnArray[i], &QClickableLabel::longPressString, this, &HomePageWidget::openBookOptionsDialog);
                bookBtnArray[i]->setAlignment(Qt::AlignCenter);
                bookBtnArray[i]->setFont(QFont("u001"));
                bookBtnArray[i]->setStyleSheet("color: black; background-color: white; border-radius: 10px; padding: 10px");
                // Book title label
                bookTitleArray[i]->setWordWrap(true);
                bookTitleArray[i]->setAlignment(Qt::AlignCenter);
                bookTitleArray[i]->setFont(QFont("u001"));
                bookTitleArray[i]->setStyleSheet("font-size: 7pt");

                QString bookTitle = uncompressedJsonObject["Title"].toString();
                bookTitleArray[i]->setObjectName(bookTitle);

                int localBookTitleTruncateThreshold;
                if(!bookTitle.contains(" ")) {
                    localBookTitleTruncateThreshold = bookTitleTruncateThreshold - 10;
                }
                else {
                    localBookTitleTruncateThreshold = bookTitleTruncateThreshold;
                }
                if(bookTitle.length() > localBookTitleTruncateThreshold) {
                    bookTitleArray[i]->setProperty("showToolTip", "true");
                    bookTitle.truncate(localBookTitleTruncateThreshold);
                    bookTitle.append("...");
                }
                bookTitleArray[i]->setText(bookTitle);

                QString bookIcon = uncompressedJsonObject["CoverPath"].toString();
                if(QFile::exists(bookIcon)) {
                    bookBtnArray[i]->setPixmap(QPixmap(bookIcon).scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio));
                }
                else {
                    bookBtnArray[i]->setPixmap(QPixmap(":/resources/cover_unavailable.png").scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }

                verticalLayoutArray[i]->addWidget(bookBtnArray[i]);
                verticalLayoutArray[i]->addWidget(bookTitleArray[i]);
            }

            if(newRow == true) {
                newRow = false;
                horizontalLayoutArray[in] = new QHBoxLayout();
                for(int n = ((i - 1) - (global::homePageWidget::recentBooksNumberPerRow - 1)); n < (((i - 1) - (global::homePageWidget::recentBooksNumberPerRow - 1)) + global::homePageWidget::recentBooksNumberPerRow); n++) {
                    horizontalLayoutArray[in]->addLayout(verticalLayoutArray[n]);
                }
                ui->booksVerticalLayout->addLayout(horizontalLayoutArray[in]);
            }
            if(!(i % global::homePageWidget::recentBooksNumberPerRow)) {
                newRow = true;
                in++;
            }
        }

        // Hiding books that don't exist/are not in the database
        // If you think this is not an efficient way to do it, try to teach the code above not to create the button if the book does not exist and arrange layouts so that it looks linear
        for(int i = 1; i <= global::homePageWidget::recentBooksNumber; i++) {
            QString bookPath = recentBooksJsonObject["Book" + QString::number(i)].toObject().value("BookPath").toString();
            if(bookPath.isEmpty()) {
                bookBtnArray[i]->hide();
                bookTitleArray[i]->hide();
                bookBtnArray[i]->deleteLater();
                bookTitleArray[i]->deleteLater();
            }
        }
    }
    else {
        log("No books found in recent books database", className);
        QLabel * noRecentBooksLabel = new QLabel(this);
        noRecentBooksLabel->setFont(QFont("Chivo"));
        noRecentBooksLabel->setAlignment(Qt::AlignCenter);
        noRecentBooksLabel->setStyleSheet("padding: 30px");
        noRecentBooksLabel->setText("Recently read books will appear here");
        ui->booksVerticalLayout->addWidget(noRecentBooksLabel);
    }

    /* Pinned books */
    if(QFile::exists(global::localLibrary::pinnedBooksDatabasePath)) {
        log("Reading pinned books database", className);
        QFile pinnedBooksDatabase(global::localLibrary::pinnedBooksDatabasePath);
        QByteArray pinnedBooksData;
        if(pinnedBooksDatabase.open(QIODevice::ReadOnly)) {
            pinnedBooksData = pinnedBooksDatabase.readAll();
            pinnedBooksDatabase.close();
        }
        else {
            QString function = __func__; log(function + ": Failed to open pinned books database file for reading at '" + pinnedBooksDatabase.fileName() + "'", className);
        }
        QJsonObject pinnedBooksJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(pinnedBooksData))).object();
        for(int i = 1; i <= global::homePageWidget::pinnedBooksNumber; i++) {
            if(!pinnedBooksJsonObject["Book" + QString::number(i)].toObject().value("BookPath").toString().isEmpty()) {
                pinnedBooksDatabaseEmpty = false;
                break;
            }
        }

        if(pinnedBooksDatabaseEmpty == false) {
            QHBoxLayout * pinnedBooksHorizontalLayoutArray = new QHBoxLayout();
            for(int i = 1; i <= global::homePageWidget::pinnedBooksNumber; i++) {
                pinnedBooksBtnArray[i] = new QClickableLabel(this);
                pinnedBooksTitleArray[i] = new QToolTipLabel(this);
                pinnedBooksVerticalLayoutArray[i] = new QVBoxLayout();

                // Book icon button
                QObject::connect(pinnedBooksBtnArray[i], &QClickableLabel::bookPath, this, &HomePageWidget::openBook);
                QObject::connect(pinnedBooksBtnArray[i], &QClickableLabel::longPressString, this, &HomePageWidget::openBookOptionsDialog);
                pinnedBooksBtnArray[i]->setAlignment(Qt::AlignCenter);
                pinnedBooksBtnArray[i]->setFont(QFont("u001"));
                pinnedBooksBtnArray[i]->setStyleSheet("color: black; background-color: white; border-radius: 10px; padding: 10px");
                // Book title label
                pinnedBooksTitleArray[i]->setWordWrap(true);
                pinnedBooksTitleArray[i]->setAlignment(Qt::AlignCenter);
                pinnedBooksTitleArray[i]->setFont(QFont("u001"));
                pinnedBooksTitleArray[i]->setStyleSheet("font-size: 7pt");

                // Iterate until we find a book matching the pinned book's "BookPath" key/value pair
                for(int in = 1; in <= databaseBooksNumber; in++) {
                    QJsonObject bookJsonObject = databaseJsonArrayList.at(in - 1).toObject();
                    QString bookPath = pinnedBooksJsonObject["Book" + QString::number(i)].toObject().value("BookPath").toString();
                    if(bookJsonObject["BookPath"].toString() == bookPath) {
                        QByteArray data = qCompress(QJsonDocument(bookJsonObject).toJson()).toBase64();
                        QString dataString = QString(data);
                        pinnedBooksBtnArray[i]->setObjectName(dataString);
                    }
                }
                QJsonObject uncompressedJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(pinnedBooksBtnArray[i]->objectName().toUtf8()))).object();

                QString bookTitle = uncompressedJsonObject["Title"].toString();
                pinnedBooksTitleArray[i]->setObjectName(bookTitle);

                int localBookTitleTruncateThreshold;
                if(!bookTitle.contains(" ")) {
                    localBookTitleTruncateThreshold = bookTitleTruncateThreshold - 10;
                }
                else {
                    localBookTitleTruncateThreshold = bookTitleTruncateThreshold;
                }
                if(bookTitle.length() > localBookTitleTruncateThreshold) {
                    pinnedBooksTitleArray[i]->setProperty("showToolTip", "true");
                    bookTitle.truncate(localBookTitleTruncateThreshold);
                    bookTitle.append("...");
                }
                pinnedBooksTitleArray[i]->setText(bookTitle);

                QString bookIcon = uncompressedJsonObject["CoverPath"].toString();
                if(QFile::exists(bookIcon)) {
                    pinnedBooksBtnArray[i]->setPixmap(QPixmap(bookIcon).scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio));
                }
                else {
                    pinnedBooksBtnArray[i]->setPixmap(QPixmap(":/resources/cover_unavailable.png").scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }

                pinnedBooksVerticalLayoutArray[i]->addWidget(pinnedBooksBtnArray[i]);
                pinnedBooksVerticalLayoutArray[i]->addWidget(pinnedBooksTitleArray[i]);
                pinnedBooksHorizontalLayoutArray->addLayout(pinnedBooksVerticalLayoutArray[i]);
            }
            ui->pinnedBooksVerticalLayout->addLayout(pinnedBooksHorizontalLayoutArray);

            // Hiding books that don't exist/are not in the database
            for(int i = 1; i <= global::homePageWidget::pinnedBooksNumber; i++) {
                QString bookPath = pinnedBooksJsonObject["Book" + QString::number(i)].toObject().value("BookPath").toString();
                if(bookPath.isEmpty()) {
                    pinnedBooksBtnArray[i]->hide();
                    pinnedBooksTitleArray[i]->hide();
                    pinnedBooksBtnArray[i]->deleteLater();
                    pinnedBooksTitleArray[i]->deleteLater();
                }
            }
        }
    }
    if(pinnedBooksDatabaseEmpty == true) {
        log("No books found in pinned books database", className);
        QLabel * noPinnedBooksLabel = new QLabel(this);
        noPinnedBooksLabel->setFont(QFont("Chivo"));
        noPinnedBooksLabel->setAlignment(Qt::AlignCenter);
        noPinnedBooksLabel->setStyleSheet("padding: 30px");
        noPinnedBooksLabel->setText("Pinned books will appear here");
        ui->pinnedBooksVerticalLayout->addWidget(noPinnedBooksLabel);
    }
    QTimer::singleShot(500, this, SLOT(refreshScreenNative()));
}

void HomePageWidget::setupDisplaySlot() {
    setupDisplay(true);
}

void HomePageWidget::setupDisplayWithDatabase() {
    global::localLibrary::headless = true;
    LocalLibraryWidget * localLibraryWidgetWindow = new LocalLibraryWidget(this);
    localLibraryWidgetWindow->setAttribute(Qt::WA_DeleteOnClose);
    localLibraryWidgetWindow->hide();
    QObject::connect(localLibraryWidgetWindow, &LocalLibraryWidget::destroyed, this, &HomePageWidget::setupDisplaySlot);
}

void HomePageWidget::showToast(QString messageToDisplay) {
    global::toast::message = messageToDisplay;
    toastWindow = new toast(this);
    toastWindow->setAttribute(Qt::WA_DeleteOnClose);
    toastWindow->show();
}

void HomePageWidget::openBookOptionsDialog(QString bookPath) {
    int bookID;

    // Finding bookID from bookPath
    log("Reading database", className);
    QFile database(global::localLibrary::databasePath);
    QByteArray data;
    if(database.open(QIODevice::ReadOnly)) {
        data = database.readAll();
        database.close();
    }
    else {
        QString function = __func__; log(function + ": Failed to open local library database file for reading at '" + database.fileName() + "'", className);
    }
    QJsonObject databaseJsonObject = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(data))).object();
    QJsonArray databaseJsonArrayList = databaseJsonObject["database"].toArray();
    int databaseBooksNumber = databaseJsonArrayList.size();
    for(int in = 1; in <= databaseBooksNumber; in++) {
        QJsonObject bookJsonObject = databaseJsonArrayList.at(in - 1).toObject();
        if(bookJsonObject["BookPath"].toString() == bookPath) {
            // Because 'bookID = bookJsonObject["BookID"].toInt();' always returns value 0 ;)
            QString bookIDStr = bookJsonObject["BookID"].toString();
            bookID = bookIDStr.toInt();
        }
    }

    log("Opening book options dialog for book with path '" + bookPath + "', ID " + QString::number(bookID), className);
    global::localLibrary::bookOptionsDialog::bookID = bookID;
    global::localLibrary::bookOptionsDialog::deleteOption = false;
    BookOptionsDialog * bookOptionsDialogWindow = new BookOptionsDialog(this);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::openLocalBookInfoDialog, this, &HomePageWidget::openLocalBookInfoDialog);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::showToast, this, &HomePageWidget::showToast);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::destroyed, this, &HomePageWidget::handlePossibleBookPin);
    bookOptionsDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    bookOptionsDialogWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    bookOptionsDialogWindow->show();
}

void HomePageWidget::openLocalBookInfoDialog() {
    global::bookInfoDialog::localInfoDialog = true;
    BookInfoDialog * bookInfoDialogWindow = new BookInfoDialog(this);
    bookInfoDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    bookInfoDialogWindow->show();
}

void HomePageWidget::handlePossibleBookPin() {
    if(global::localLibrary::bookOptionsDialog::bookPinAction == true) {
        QTimer::singleShot(3100, this, [&]() {
            emit relaunchHomePageWidget();
        });
    }
}
