#ifndef BOOKINFODIALOG_H
#define BOOKINFODIALOG_H

#include <QDialog>

#include "textwidget.h"

namespace Ui {
class BookInfoDialog;
}

class BookInfoDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit BookInfoDialog(QWidget *parent = nullptr);
    ~BookInfoDialog();
    float sH;
    float sW;
    float stdIconHeight;
    float stdIconWidth;

private slots:
    void on_closeBtn_clicked();
    void on_getBtn_clicked();
    void waitForBookFetch();

signals:
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();

private:
    Ui::BookInfoDialog * ui;
    textwidget * textwidgetWindow;
};

#endif // BOOKINFODIALOG_H
