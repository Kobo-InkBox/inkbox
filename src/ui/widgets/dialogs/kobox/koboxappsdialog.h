#ifndef KOBOXAPPSDIALOG_H
#define KOBOXAPPSDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "usbmsSplash.h"

namespace Ui {
class KoboxAppsDialog;
}

class KoboxAppsDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit KoboxAppsDialog(QWidget *parent = nullptr);
    ~KoboxAppsDialog();

    void checkApps();

    QString apps;
    QString itemText;
    QString dpiSetting;
    QString dpModeSetting;
    QModelIndex index;

private slots:
    void on_cancelBtn_clicked();
    void on_launchBtn_clicked();

signals:
    void showToast(QString messageToDisplay);

private:
    Ui::KoboxAppsDialog *ui;
    USBMSSplashWidget *usbmsSplashWindow;
};

#endif // KOBOXAPPSDIALOG_H
