#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>

#include "functions.h"

namespace Ui {
class TextDialog;
}

class TextDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit TextDialog(QWidget *parent = nullptr);
    ~TextDialog();
    bool highlightAlreadyDone = false;

signals:
    void highlightText();
    void unhighlightText();

private slots:
    void on_highlightBtn_clicked();

private:
    Ui::TextDialog *ui;
};

#endif // TEXTDIALOG_H
