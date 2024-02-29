#include "locallibrarywidget.h"
#include "ui_locallibrarywidget.h"
#include "device.h"

#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScreen>

LocalLibraryWidget::LocalLibraryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalLibraryWidget)
{
    Device *device = Device::getSingleton();
    Device::ScreenSize screenSize = device->getScreenSize();

    ui->setupUi(this);
    this->setFont(QFont("u001"));
    QObject::connect(ui->pageNumberLabel, &QClickableLabel::clicked, this, &LocalLibraryWidget::openGoToPageDialog);

    ui->previousPageBtn->setProperty("type", "borderless");
    ui->previousPageBtn->setEnabled(false);
    ui->nextPageBtn->setProperty("type", "borderless");
    ui->goUpBtn->setProperty("type", "borderless");
    ui->pathBtn->setProperty("type", "borderless");
    ui->previousPageBtn->setIcon(QIcon(":/resources/chevron-left.png"));
    ui->nextPageBtn->setIcon(QIcon(":/resources/chevron-right.png"));
    ui->pageNumberLabel->setFont(QFont("Source Serif Pro"));
    ui->pageNumberLabel->setStyleSheet("color: black; background-color: white; border-radius: 10px; padding-left: 10px; padding-right: 10px");

    ui->goUpBtn->setEnabled(false);

    if(screenSize == Device::ScreenSize::SMALL) {
        buttonsNumber = 3;
    }
    else if(screenSize >= Device::ScreenSize::VERY_LARGE) {
        buttonsNumber = 5;
    }
    else {
        buttonsNumber = 4;
    }

    if(screenSize == Device::ScreenSize::SMALL) {
        bookTitleTruncateThreshold = 27;
    }
    else if(screenSize >= Device::ScreenSize::VERY_LARGE) {
        bookTitleTruncateThreshold = 45;
    }
    else if(screenSize == Device::ScreenSize::MEDIUM) {
        bookTitleTruncateThreshold = 32;
    }
    else {
        bookTitleTruncateThreshold = 35;
    }

    bookIconArray.resize(buttonsNumber);
    bookBtnArray.resize(buttonsNumber);
    horizontalLayoutArray.resize(buttonsNumber);
    lineArray.resize(buttonsNumber);

    // Getting the screen's size
    sW = QGuiApplication::screens()[0]->size().width();
    sH = QGuiApplication::screens()[0]->size().height();


    if(screenSize == Device::ScreenSize::SMALL) {
        stdIconWidthDivider = 7.2;
        stdIconHeightDivider = 7.2;
    } else if(screenSize == Device::ScreenSize::MEDIUM) {
        stdIconWidthDivider = 8.1;
        stdIconHeightDivider = 8.1;
    } else if(screenSize == Device::ScreenSize::LARGE) {
        stdIconWidthDivider = 8.5;
        stdIconHeightDivider = 8.5;
    } else if(screenSize >= Device::ScreenSize::VERY_LARGE) {
        stdIconWidthDivider = 9.7;
        stdIconHeightDivider = 9.7;
    }

    stdIconWidth = sW / stdIconWidthDivider;
    stdIconHeight = sH / stdIconHeightDivider;

    setupButtonsLook();

    if(!QFile::exists(global::localLibrary::databasePath)) {
        global::toast::modalToast = true;
        global::toast::indefiniteToast = true;
        showToast("Generating database");
    }
    QTimer::singleShot(100, this, SLOT(setupDisplay()));

    if(checkconfig("/mnt/onboard/.adds/inkbox/.config/21-local_library/folders") == false) {
        folderFeatureEnabled = false;
        ui->goUpBtn->hide();
        ui->pathBtn->hide();
        ui->goUpBtn->deleteLater();
        ui->pathBtn->deleteLater();
    }
    else {
        folderFeatureEnabled = true;
    }
}

LocalLibraryWidget::~LocalLibraryWidget()
{
    delete ui;
}

void LocalLibraryWidget::setupDatabase() {
    setDefaultWorkDir();
    if(!QFile::exists(global::localLibrary::databasePath)) {
        log("Generating database", className);
        QStringList booksList;
        QDir dir("/mnt/onboard/onboard");

        QDirIterator dirIt(dir, QDirIterator::Subdirectories);
        while(dirIt.hasNext()) {
            dirIt.next();
            if(QFileInfo(dirIt.filePath()).isFile()) {
                QString suffix = QFileInfo(dirIt.filePath()).suffix();
                if(suffix == "epub" || suffix == "EPUB") {
                    booksList << dirIt.filePath();
                }
            }
        }

        QString prog("busybox-initrd");
        QStringList args;
        args << "env" << "icon_width_divider=" + QString::number(stdIconWidthDivider - 1.5) << "icon_height_divider=" + QString::number(stdIconHeightDivider - 1.5) << "./explore_local_library.sh" << booksList;

        /*
        *   Logs/steps needed to debug the database creation
        *   for(int i = 0; i < args.count(); i++) {
        *       log("Arguments for database creation: '" + args[i] + "'", className);
        *   }
        */

        QProcess *proc = new QProcess();
        proc->start(prog, args);
        proc->waitForFinished(-1);
        QJsonDocument jsonDocument = QJsonDocument::fromJson(readFile(global::localLibrary::rawDatabasePath).toUtf8());
        // log("stdout of process: " + proc->readAllStandardOutput(), className);
        // log("stderr of process: " + proc->readAllStandardError(), className);
        QFile::remove(global::localLibrary::rawDatabasePath);
        proc->deleteLater();

        // Write database in compressed form, encoded in Base64 format
        writeFile(global::localLibrary::databasePath, qCompress(jsonDocument.toJson()).toBase64());
        toastWindow->close();
    }

    // Read library database from file
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

    // Uncompress data from Base64 encoding
    databaseJsonDocument = QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(data)));
    // Parse JSON data
    databaseJsonObject = databaseJsonDocument.object();
    databaseJsonArrayList = databaseJsonObject["database"].toArray();
    // Determine maximum page number
    booksNumber = databaseJsonArrayList.size();
    if(folderFeatureEnabled == false) {
        pagesNumber = std::ceil((double)booksNumber / buttonsNumber);
    }
    else {
        calculateMaximumPagesNumberForFolders();
    }
    if(databaseJsonArrayList.isEmpty()) {
        log("Database is empty", className);
        noBooksInDatabase = true;
    }
    else {
        noBooksInDatabase = false;
        if(pagesNumber == 1) {
            ui->nextPageBtn->setEnabled(false);
        }
    }

    if(global::localLibrary::headless == true) {
        global::localLibrary::headless = false;
        LocalLibraryWidget::close();
    }
}

void LocalLibraryWidget::setupBooksList(int pageNumber) {
    idList.clear();
    int in = 1;
    for(int i = (1 * pageNumber * buttonsNumber) - (buttonsNumber - 1); i <= (1 * pageNumber * buttonsNumber); i++) {
        // Read database info for each book and display the corresponding information on each button
        bookIconArray[in]->show();
        bookBtnArray[in]->show();
        if(buttonsNumber - in > 0) {
            lineArray[in]->show();
        }

        QJsonObject jsonObject = databaseJsonArrayList.at(i - 1).toObject();
        QString bookTitle = jsonObject["Title"].toString();
        QString bookAuthor = jsonObject["Author"].toString();
        QString coverPath = jsonObject["CoverPath"].toString();
        QString bookID = jsonObject["BookID"].toString();

        if(!coverPath.isEmpty()) {
            // Display book cover if found
            QPixmap pixmap(coverPath);
            bookIconArray[in]->setPixmap(pixmap.scaled(stdIconWidth, stdIconHeight, Qt::IgnoreAspectRatio));
        }
        else {
            QPixmap pixmap(":/resources/cover_unavailable.png");
            bookIconArray[in]->setPixmap(pixmap.scaled(stdIconWidth, stdIconHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }

        // Display book title
        idList.append(bookID.toInt());
        if(bookTitle.length() > bookTitleTruncateThreshold) {
            bookTitle.truncate(bookTitleTruncateThreshold);
            bookTitle.append("...");
        }
        if(!bookAuthor.isEmpty()) {
            // Book is likely an ePUB
            bookBtnArray[in]->setText("<font face='Inter'><b>" + bookTitle + "</b></font>" + "<br>" + bookAuthor);
        }
        else {
            // Book is likely a PDF or a picture
            bookBtnArray[in]->setText("<font face='Inter'><b>" + bookTitle + "</b></font>");
        }

        if(!bookID.isEmpty()) {
            in++;
        }
    }
    if(in <= buttonsNumber) {
        for(int i = in; i <= buttonsNumber; i++) {
            bookIconArray[i]->hide();
            bookBtnArray[i]->hide();
            if(i - 1 < buttonsNumber) {
                lineArray[(i - 1)]->hide();
            }
        }
    }
    ui->pageNumberLabel->setText(QString::number(pageNumber) + " <i>of</i> " + QString::number(pagesNumber));
    // NOTICE: Memory leak?
    // Do it twice, otherwise the layout doesn't show as intended
    for(int i = 0; i <= 1; i++) {
        ui->verticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    }

    // Set boundaries for 'Previous'/'Next' page turn buttons
    currentPageNumber = pageNumber;
    ui->previousPageBtn->setEnabled(true);
    ui->nextPageBtn->setEnabled(true);
    if(currentPageNumber - 1 < 1) {
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(true);
    }
    if(currentPageNumber + 1 > pagesNumber) {
        ui->previousPageBtn->setEnabled(true);
        ui->nextPageBtn->setEnabled(false);
    }
    if(currentPageNumber - 1 < 1 and currentPageNumber + 1 > pagesNumber) {
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(false);
    }
}

void LocalLibraryWidget::on_previousPageBtn_clicked()
{
    log("Previous button clicked", className);

    if(folderFeatureEnabled == true) {
        log("Decreasing bookIndexVector by buttonsNumber", className);
        bookIndexVector = (bookIndexVector - goBackInIndex) - buttonsNumber;
        if(bookIndexVector < 0) {
            log("Error: Shouldn't be possible to get here", className);
            bookIndexVector = 0;
        }
    }

    currentPageNumber--;
    setupBooksListToggle(currentPageNumber);

    pagesTurned = pagesTurned + 1;
    if(pagesTurned >= 3) {
        emit refreshScreen();
        pagesTurned = 0;
    }
}


void LocalLibraryWidget::on_nextPageBtn_clicked()
{
    currentPageNumber++;
    setupBooksListToggle(currentPageNumber);

    pagesTurned = pagesTurned + 1;
    if(pagesTurned >= 3) {
        emit refreshScreen();
        pagesTurned = 0;
    }
}

void LocalLibraryWidget::openBook(int bookID) {
    QJsonObject jsonObject = databaseJsonArrayList.at(bookID - 1).toObject();
    QString bookPath = jsonObject["BookPath"].toString();
    log("Opening book with ID " + QString::number(bookID) + ", path '" + bookPath + "'", className);
    emit openBookSignal(bookPath, false);
}

void LocalLibraryWidget::btnOpenBook(int buttonNumber) {
    log("Book/directory button clicked, buttonNumber is " + QString::number(buttonNumber), className);
    if(mainPathIsEmpty == true) {
        return void();
    }

    int id = idList.at(buttonNumber - 1);
    if(id == global::localLibrary::folderID) {
        if(folderFeatureEnabled == true) {
            log("A folder was selected", className);
            QString directory = bookBtnArray[buttonNumber]->statusTip();
            changePathAndRefresh(directory);
        }
    }
    else {
        log("A book was selected", className);
        openBook(id);
        LocalLibraryWidget::close();
    }
}

void LocalLibraryWidget::openGoToPageDialog() {
    if(mainPathIsEmpty == true) {
        return void();
    }

    log("Showing 'Go to page' dialog", className);
    global::keyboard::keypadDialog = true;
    generalDialogWindow = new GeneralDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(generalDialogWindow, &GeneralDialog::gotoPageSelected, this, &LocalLibraryWidget::goToPage);
}

void LocalLibraryWidget::goToPage(int page) {
    checkIfMainPathIsEmpty();
    if(mainPathIsEmpty == true) {
        return void();
    }

    if(page > pagesNumber or page <= 0) {
        log("Page number specified (" + QString::number(page) + ") is out of range", className);
        showToast("Request is beyond page range");
    }
    else {
        if(folderFeatureEnabled == true) {
            calculateIndexForPage(page);
        }
        log("Going to page " + QString::number(page), className);
        setupBooksListToggle(page);
        emit refreshScreen();
    }
}

void LocalLibraryWidget::refreshScreenNative() {
    emit refreshScreen();
}

void LocalLibraryWidget::setupDisplay() {
    setupDatabase();
    if(noBooksInDatabase == false) {
        // Prevent segmentation fault if a book was the last of its page
        int pageToGo = currentPageNumber;
        if(currentPageNumber > pagesNumber) {
            pageToGo = pageToGo - 1;
        }
        if(global::localLibrary::bookOptionsDialog::bookDeleted == true and folderFeatureEnabled == true) {
            goToPage(pageToGo);
        }
        else {
            setupBooksListToggle(pageToGo);
        }
    }
    else {
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(false);
        ui->pageNumberLabel->setText("1 <i>of</i> 1");
        ui->stackedWidget->setCurrentIndex(1);
    }
    global::localLibrary::bookOptionsDialog::bookDeleted = false;
}

void LocalLibraryWidget::showToast(QString messageToDisplay) {
    global::toast::message = messageToDisplay;
    toastWindow = new toast(this);
    toastWindow->setAttribute(Qt::WA_DeleteOnClose);
    toastWindow->show();
}

void LocalLibraryWidget::openBookOptionsDialog(int pseudoBookID) {
    if(mainPathIsEmpty == true) {
        return void();
    }

    // Determine book ID from the book's button number
    // pseudoBookID represents the button's number
    int bookID = ((currentPageNumber * buttonsNumber) - (buttonsNumber - 1)) + (pseudoBookID - 1);
    log("BookID is " + QString::number(bookID), className);
    int id = idList.at(pseudoBookID - 1);
    log("ID from database is " + QString::number(id), className);

    if(id == global::localLibrary::folderID) {
        if(folderFeatureEnabled == true) {
            bookID = id;
            log("Opening options dialog for directory", className);
            QString directoryPath = bookBtnArray[pseudoBookID]->statusTip();
            log("Directory path is '" + directoryPath + "'", className);
            global::localLibrary::bookOptionsDialog::isFolder = true;
            global::localLibrary::bookOptionsDialog::folderPath = pathForFolders + directoryPath;
        }
    }
    else {
        global::localLibrary::bookOptionsDialog::isFolder = false;
        global::localLibrary::bookOptionsDialog::folderPath = "";
    }

    log("Opening book options dialog for book with pseudo-ID " + QString::number(pseudoBookID) + ", ID " + QString::number(bookID), className);
    // Here, ID should go because of idList.at(pseudoBookID - 1);
    global::localLibrary::bookOptionsDialog::bookID = id;
    BookOptionsDialog * bookOptionsDialogWindow = new BookOptionsDialog(this);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::openLocalBookInfoDialog, this, &LocalLibraryWidget::openLocalBookInfoDialog);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::showToast, this, &LocalLibraryWidget::showToast);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::destroyed, this, &LocalLibraryWidget::handlePossibleBookDeletion);
    QObject::connect(bookOptionsDialogWindow, &BookOptionsDialog::removedFolder, this, &LocalLibraryWidget::refreshFolders);
    bookOptionsDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    bookOptionsDialogWindow->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    bookOptionsDialogWindow->show();
}

void LocalLibraryWidget::handlePossibleBookDeletion() {
    if(global::localLibrary::bookOptionsDialog::bookDeleted == true) {
        QTimer::singleShot(3100, this, [&]() {
            global::toast::modalToast = true;
            global::toast::indefiniteToast = true;
            showToast("Generating database");
            QTimer::singleShot(100, this, SLOT(setupDisplay()));
        });
    }
}

void LocalLibraryWidget::openLocalBookInfoDialog() {
    global::bookInfoDialog::localInfoDialog = true;
    BookInfoDialog * bookInfoDialogWindow = new BookInfoDialog(this);
    bookInfoDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    bookInfoDialogWindow->show();
}

void LocalLibraryWidget::setupBooksListToggle(int pageNumber) {
    if(folderFeatureEnabled == true) {
        setupBooksListFolders(pageNumber);
    }
    else {
        setupBooksList(pageNumber);
    }
}

void LocalLibraryWidget::setupBooksListFolders(int pageNumber) {
    log("Showing folders for page: " + QString::number(pageNumber), className);
    QStringList directoryList = QDir(pathForFolders).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    log("Full directory list is '" + directoryList.join(", ") + "'", className);

    if(mainPathIsEmpty == true) {
        // Main path is set and is empty
        ui->pageNumberLabel->setText("0 <i>of</i> 0");
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(false);
        return void();
    }

    // This part is calculating which folders to show per page
    QStringList directoryListFront = directoryList;
    QStringList directoryListBack = directoryList;
    int pageNumberAbove = pageNumber;
    while(pageNumberAbove != 1) {
        for (int i = 0; i < buttonsNumber; ++i) {
            if(directoryListFront.isEmpty() == false) {
                directoryListFront.removeFirst();
            }
        }
        pageNumberAbove = pageNumberAbove - 1;
    }
    log("Front directory list is '" + directoryListFront.join(", ") + "'", className);

    int aboveRemove = pageNumber * buttonsNumber;
    if(directoryListBack.count() > aboveRemove) {
        while(directoryListBack.count() > aboveRemove) {
            if(directoryListBack.isEmpty() == false) {
                directoryListBack.removeLast();
            }
        }
    }
    log("Back directory list is '" + directoryListBack.join(", ") + "'", className);

    QStringList directoryListPure;
    for(QString directory: directoryListFront) {
        if(directoryListBack.contains(directory) == true) {
            directoryListPure.append(directory);
        }
    }
    log("Final directory list is '" + directoryListPure.join(", ") +"'", className);

    idList.clear();
    int in = 1;
    int directoryCount = 0; // I want to start at 0
    goBackInIndex = 0;
    for(int i = (1 * pageNumber * buttonsNumber) - (buttonsNumber - 1); i <= (1 * pageNumber * buttonsNumber); i++) {
        if(directoryListPure.count() != directoryCount) {
            // Insert a folder if here
            log("Showing a folder for index " + QString::number(i), className);

            // Show it, it may be hidden
            bookIconArray[in]->show();
            bookBtnArray[in]->show();
            if(buttonsNumber - in > 0) {
                lineArray[in]->show();
            }

            QString directoryNameFull = directoryListPure.at(directoryCount);
            bookBtnArray[in]->setStatusTip(directoryNameFull);
            QString directoryNameNotFull = directoryNameFull;
            if(directoryNameNotFull.length() > bookTitleTruncateThreshold) {
                directoryNameNotFull.truncate(bookTitleTruncateThreshold);
                directoryNameNotFull.append("...");
            }
            bookBtnArray[in]->setText("<font face='Inter'><b>" + directoryNameNotFull + "</b></font>");
            bookIconArray[in]->setPixmap(pixmapForFolder.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // ID for indicating that it's a folder in btnOpenBook
            idList.append(global::localLibrary::folderID);
            directoryCount = directoryCount + 1;
            in++;
        }
        else {
            // Show a book otherwise
            log("bookIndexVector used: " + QString::number(bookIndexVector), className);
            log("Showing a book for index: " + QString::number(i), className);
            // Read database info for each book and display the corresponding information on each button
            bookIconArray[in]->show();
            bookBtnArray[in]->show();
            if(buttonsNumber - in > 0) {
                lineArray[in]->show();
            }

            // If a book is missing, it's propably because of '>='
            if(bookIndexVector >= booksListForPathIndex.count()) {
                continue;
            }

            QJsonObject jsonObject = databaseJsonArrayList.at(booksListForPathIndex.at(bookIndexVector)).toObject();
            QString bookTitle = jsonObject["Title"].toString();
            QString bookAuthor = jsonObject["Author"].toString();
            QString coverPath = jsonObject["CoverPath"].toString();
            QString bookID = jsonObject["BookID"].toString();

            if(QFile(coverPath).exists()) {
                // Display book cover if found
                QPixmap pixmap(coverPath);
                bookIconArray[in]->setPixmap(pixmap.scaled(stdIconWidth, stdIconHeight, Qt::IgnoreAspectRatio));
            }
            else {
                QPixmap pixmap(":/resources/cover_unavailable.png");
                bookIconArray[in]->setPixmap(pixmap.scaled(stdIconWidth, stdIconHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }

            // Display book title
            idList.append(bookID.toInt());
            if(bookTitle.length() > bookTitleTruncateThreshold) {
                bookTitle.truncate(bookTitleTruncateThreshold);
                bookTitle.append("...");
            }
            if(!bookAuthor.isEmpty()) {
                // Book is likely an ePUB
                bookBtnArray[in]->setText("<font face='Inter'><b>" + bookTitle + "</b></font>" + "<br>" + bookAuthor);
            }
            else {
                // Book is likely a PDF or a picture
                bookBtnArray[in]->setText("<font face='Inter'><b>" + bookTitle + "</b></font>");
            }
            if(!bookID.isEmpty()) {
                in++;
            }
            bookIndexVector = bookIndexVector + 1;
        }
        goBackInIndex = goBackInIndex + 1;
    }

    if(in <= buttonsNumber) {
        for(int i = in; i <= buttonsNumber; i++) {
            log("Hiding items", className);
            bookIconArray[i]->hide();
            bookBtnArray[i]->hide();
            if(i - 1 < buttonsNumber) {
                lineArray[(i - 1)]->hide();
            }
        }
    }

    ui->pageNumberLabel->setText(QString::number(pageNumber) + " <i>of</i> " + QString::number(pagesNumber));
    // NOTICE: Memory leak?
    // Do it twice, otherwise the layout doesn't show as intended
    for(int i = 0; i <= 1; i++) {
        ui->verticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    }

    // Set boundaries for 'Previous'/'Next' page turn buttons
    currentPageNumber = pageNumber;
    ui->previousPageBtn->setEnabled(true);
    ui->nextPageBtn->setEnabled(true);
    if(currentPageNumber - 1 < 1) {
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(true);
    }
    if(currentPageNumber + 1 > pagesNumber) {
        ui->previousPageBtn->setEnabled(true);
        ui->nextPageBtn->setEnabled(false);
    }
    if(currentPageNumber - 1 < 1 and currentPageNumber + 1 > pagesNumber) {
        ui->previousPageBtn->setEnabled(false);
        ui->nextPageBtn->setEnabled(false);
    }
}

// Sets pagesNumber for folders feature, and everything else
void LocalLibraryWidget::calculateMaximumPagesNumberForFolders() {
    log("Main path is '" + pathForFolders + "'", className);

    checkIfMainPathIsEmpty();
    if(mainPathIsEmpty == true) {
        return void();
    }

    // Look for books in this path
    booksListForPathIndex.clear();
    int count = 0;
    for(QJsonValue object: databaseJsonArrayList) {
        QString bookPath = object.toObject()["BookPath"].toString();
        QString bookDirPath = QFileInfo(bookPath).absoluteDir().path();

        if(bookDirPath.at(bookDirPath.count() - 1) == "/") {
            bookDirPath = bookDirPath.remove(bookDirPath.count() - 1, 1);
        }

        QString temporaryPathForFolders = pathForFolders;
        if(temporaryPathForFolders.at(temporaryPathForFolders.count() - 1) == "/") {
            temporaryPathForFolders = temporaryPathForFolders.remove(temporaryPathForFolders.count() - 1, 1);
        }

        if(bookDirPath == temporaryPathForFolders) {
            booksListForPathIndex.append(count);
        }
        count = count + 1;
    }
    QStringList list;
    foreach (int number, booksListForPathIndex) {
        list.append(QString::number(number));
    }
    log("bookListForPathIndex is '" + list.join(", ") + "'", className);

    directoryListCount = QDir(pathForFolders).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name).count();
    log("Directories count in directory: " + QString::number(directoryListCount), className);
    completeItemsList = booksListForPathIndex.count() + directoryListCount;

    log("All items: " + QString::number(completeItemsList), className);

    // Fix for bug:
    /*
     put files in a folder, then delete all files in that folder from the GUI.
     Then the bug happens: "page request is out of range" or something like that
    */
    if(completeItemsList == 0) {
        log("No items in this folder because they were deleted. Going up", className);
        goUpFunction();
    }

    pagesNumber = std::ceil((double)completeItemsList / buttonsNumber);

    log("Total pages: " + QString::number(pagesNumber), className);

    // This is the last page full of folders
    firstPageForBooks = (QString::number(float(directoryListCount) / buttonsNumber).split(".").first()).toInt();
    log("There are so many pages with folders: " + QString::number(firstPageForBooks), className);

    // This indicates how much folders are after firstPageForBooks. It's always less than buttonsNumber
    lastPageFolderCount = directoryListCount;
    while(lastPageFolderCount >= buttonsNumber) {
        lastPageFolderCount = lastPageFolderCount - buttonsNumber;
    }
    log("Start after item on the last page: " + QString::number(lastPageFolderCount), className);

    // Sorting the vector if needed should be done here
}

void LocalLibraryWidget::calculateIndexForPage(int pageNumber) {
    if(pageNumber == firstPageForBooks + 1) {
        log("Variable firstPageForBooks is 0", className);
        bookIndexVector = 0;
        return void();
    }

    if(pageNumber == firstPageForBooks + 2) {
        log("The page is just after firstPageForBooks", className);
        bookIndexVector = buttonsNumber - lastPageFolderCount;
        return void();
    }
    else if(pageNumber > firstPageForBooks + 2) {
        bookIndexVector = buttonsNumber - lastPageFolderCount;
        int pageUntilGoal = 1;
        int pageDifference = pageNumber - (firstPageForBooks + 2);
        log("Variable firstPageForBooks is " + QString::number(firstPageForBooks), className);
        log("Variable pageDifference is " + QString::number(pageDifference), className);
        if(pageDifference != 0) {
            while(pageUntilGoal != pageDifference) {
                log("Looping for pageUntilGoal");
                bookIndexVector = bookIndexVector + buttonsNumber;
                pageUntilGoal++;
            }
        }
        bookIndexVector = bookIndexVector + buttonsNumber;
        log("Calculated bookIndexVector is " + QString::number(bookIndexVector), className);
    }
    else {
        log("Variable pageNumber isn't higher than firstPageForBooks", className);
        bookIndexVector = 0;
    }
}

void LocalLibraryWidget::changePathAndRefresh(QString directory) {
    log("Changing path", className);
    QString temporaryPathForFolders = pathForFolders + directory + "/";
    if(QDir(temporaryPathForFolders).isEmpty() == false) {
        pathForFolders = temporaryPathForFolders;
        calculateMaximumPagesNumberForFolders();
        bookIndexVector = 0;
        ui->goUpBtn->setEnabled(true);
        goToPage(1);
    }
    else {
        showToast("Directory is empty");
    }
}

void LocalLibraryWidget::on_goUpBtn_clicked()
{
    goUpFunction();
}

void LocalLibraryWidget::goUpFunction() {
    if(pathForFolders != "/mnt/onboard/onboard/") {
        log("Changing path; going back", className);
        // This can't be a one-liner
        QDir temporaryPathForFolders = QDir(pathForFolders);
        temporaryPathForFolders.cdUp();
        pathForFolders = temporaryPathForFolders.path();
        pathForFolders = pathForFolders + "/";
        if(pathForFolders == "/mnt/onboard/onboard/") {
            ui->goUpBtn->setDisabled(true);
        }
        log("New path is '" + pathForFolders + "'", className);

        calculateMaximumPagesNumberForFolders();
        bookIndexVector = 0;
        goToPage(1);
    }
}

void LocalLibraryWidget::on_pathBtn_clicked()
{
    log("Showing path dialog", className);
    QString pathForFoldersSaved = pathForFolders;
    showToast(pathForFolders.remove(0, 20));
    pathForFolders = pathForFoldersSaved;
}

void LocalLibraryWidget::refreshFolders() {
    log("Called refresh folders");
    calculateMaximumPagesNumberForFolders();
    bookIndexVector = 0;
    goToPage(1);
}

void LocalLibraryWidget::checkIfMainPathIsEmpty() {
    if(folderFeatureEnabled == true) {
        // If the main path is empty, prevent it from freezing and using the CPU at 100% because of a while loop
        if(pathForFolders == "/mnt/onboard/onboard/") {
            bool isDirectoryEmpty = QDir(pathForFolders).entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot).isEmpty();
            log("Main path is empty: " + QVariant(isDirectoryEmpty).toString(), className);
            if(isDirectoryEmpty == true) {
                // To clean things out after a deletion
                cleanButtons();
                booksListForPathIndex.clear();
                directoryListCount = 0;
                completeItemsList = 0;
                pagesNumber = 0;
                firstPageForBooks = 0;
                lastPageFolderCount = 0;
                mainPathIsEmpty = true;
                showToast("Library is empty");
                return void();
            }
            else {
                mainPathIsEmpty = false;
            }
        }
    }
}

void LocalLibraryWidget::setupButtonsLook() {
    for(int i = 0; i < buttonsNumber; i++) {
        // Horizontal layout that will contain the book button and its icon
        horizontalLayoutArray[i] = new QHBoxLayout(this);

        // Book icon label
        bookIconArray[i] = new QLabel(this);
        bookIconArray[i]->setStyleSheet("padding: 10px");

        // Book button
        bookBtnArray[i] = new QClickableLabel(this);
        bookBtnArray[i]->setObjectName(QString::number(i));
        QObject::connect(bookBtnArray[i], &QClickableLabel::bookID, this, &LocalLibraryWidget::btnOpenBook);
        QObject::connect(bookBtnArray[i], &QClickableLabel::longPressInt, this, &LocalLibraryWidget::openBookOptionsDialog);
        bookBtnArray[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        bookBtnArray[i]->setStyleSheet("color: black; background-color: white; border-radius: 10px; padding: 10px");
        bookBtnArray[i]->setFont(QFont("u001"));

        horizontalLayoutArray[i]->addWidget(bookIconArray[i]);
        horizontalLayoutArray[i]->addWidget(bookBtnArray[i]);
        ui->booksVerticalLayout->addLayout(horizontalLayoutArray[i]);

        // Line
        if(i < buttonsNumber) {
            lineArray[i] = new QFrame(this);
            lineArray[i]->setFrameShape(QFrame::HLine);
            lineArray[i]->setFrameShadow(QFrame::Plain);
            lineArray[i]->setLineWidth(1);
            ui->booksVerticalLayout->addWidget(lineArray[i]);
        }
    }
}

void LocalLibraryWidget::cleanButtons() {
    log("Hiding items", className);
    for(int i = 1; i <= buttonsNumber; i++) {
        if(bookIconArray[i]->isHidden() == false) {
            bookIconArray[i]->hide();
        }
        if(bookBtnArray[i]->isHidden() == false) {
            bookBtnArray[i]->hide();
        }
    }
}
