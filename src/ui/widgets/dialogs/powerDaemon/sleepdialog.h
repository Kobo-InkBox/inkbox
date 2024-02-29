#ifndef SLEEPDIALOG_H
#define SLEEPDIALOG_H

#include <QDialog>

namespace Ui {
class SleepDialog;
}

class SleepDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit SleepDialog(QDialog *parent = nullptr);
    ~SleepDialog();

public slots:
    void launchSleepDialog();
    void hideSleepDialog();


private:
    Ui::SleepDialog * ui;
};

#endif // SLEEPDIALOG_H
