#ifndef LOCALLIBRARYWIDGET_H
#define LOCALLIBRARYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include "functions.h"
#include "qclickablelabel.h"
#include "generaldialog.h"
#include "toast.h"
#include "bookoptionsdialog.h"
#include "bookinfodialog.h"

namespace Ui {
class LocalLibraryWidget;
}

class LocalLibraryWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit LocalLibraryWidget(QWidget *parent = nullptr);
    ~LocalLibraryWidget();
    int buttonsNumber;
    int sW;
    int sH;
    float stdIconWidthDivider;
    float stdIconHeightDivider;
    int stdIconWidth;
    int stdIconHeight;
    QJsonDocument databaseJsonDocument;
    QJsonObject databaseJsonObject;
    QJsonArray databaseJsonArrayList;
    int currentPageNumber = 1;
    int pagesNumber;
    int pagesTurned = 0;
    int booksNumber;
    int bookTitleTruncateThreshold;
    QVector<int> idList;
    bool noBooksInDatabase = true;
    // For folder feature
    QString pathForFolders = "/mnt/onboard/onboard/";
    bool folderFeatureEnabled = false;
    int firstPageForBooks;
    int lastPageFolderCount;
    int bookIndexVector = 0;
    int goBackInIndex = 0;
    QPixmap pixmapForFolder = QPixmap(":/resources/folder.png");
    QVector<int> booksListForPathIndex;
    int fileListCount;
    int directoryListCount;
    int completeItemsList;
    // If the path is set to '/mnt/onboard/onboard/', and it's empty, this will be true
    bool mainPathIsEmpty = false;

private slots:
    void setupDatabase();
    void setupBooksList(int pageNumber);
    void on_previousPageBtn_clicked();
    void on_nextPageBtn_clicked();
    void openBook(int id);
    void btnOpenBook(int buttonNumber);
    void refreshScreenNative();
    void openGoToPageDialog();
    void goToPage(int page);
    void setupDisplay();
    void showToast(QString messageToDisplay);
    void openBookOptionsDialog(int pseudoBookID);
    void handlePossibleBookDeletion();
    void openLocalBookInfoDialog();
    void setupButtonsLook();
    void cleanButtons();
    // For folder feature
    void setupBooksListFolders(int pageNumber);
    void setupBooksListToggle(int pageNumber);
    void calculateMaximumPagesNumberForFolders();
    void calculateIndexForPage(int pageNumber);
    void goUpFunction();
    void checkIfMainPathIsEmpty();
    // Dir without "/" at the end and begining
    void changePathAndRefresh(QString directory);
    void refreshFolders();
    void on_goUpBtn_clicked();
    void on_pathBtn_clicked();

private:
    Ui::LocalLibraryWidget * ui;
    GeneralDialog * generalDialogWindow;
    BookOptionsDialog * bookOptionsDialogWindow;
    toast * toastWindow;
    BookInfoDialog * bookInfoDialogWindow;
    QVector<QHBoxLayout*> horizontalLayoutArray;
    QVector<QLabel*> bookIconArray;
    QVector<QClickableLabel*> bookBtnArray;
    QVector<QFrame*> lineArray;

signals:
    void openBookSignal(QString bookFile, bool relativePath);
    void refreshScreen();
};

#endif // LOCALLIBRARYWIDGET_H
