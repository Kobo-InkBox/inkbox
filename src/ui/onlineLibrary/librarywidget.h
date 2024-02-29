#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QWidget>
#include <toast.h>
#include <bookinfodialog.h>

namespace Ui {
class LibraryWidget;
}

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit LibraryWidget(QWidget *parent = nullptr);
    ~LibraryWidget();
    float sH;
    float sW;
    float stdIconHeight;
    float stdIconWidth;

private:
    Ui::LibraryWidget * ui;
    toast * toastWindow;
    BookInfoDialog * bookInfoDialogWindow;

private slots:
    void syncCatalog();
    void syncCatalogSlot();
    void setupView();
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();
    QString getTitle(int bookNumber);
    void openLatestBookInfoDialog(int bookNumber, QString title);

    void on_previousBtn_clicked();
    void on_nextBtn_clicked();
    void on_book1Btn_clicked();
    void on_book2Btn_clicked();
    void on_book3Btn_clicked();
    void on_book4Btn_clicked();
    void on_book5Btn_clicked();
    void on_book6Btn_clicked();
    void on_book7Btn_clicked();
    void on_book8Btn_clicked();
    void on_book9Btn_clicked();
    void on_book10Btn_clicked();
    void on_book11Btn_clicked();
    void on_book12Btn_clicked();
    void on_book13Btn_clicked();
    void on_book14Btn_clicked();
    void on_book15Btn_clicked();
    void on_book16Btn_clicked();
};

#endif // LIBRARYWIDGET_H
