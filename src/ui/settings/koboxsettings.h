#ifndef KOBOXSETTINGS_H
#define KOBOXSETTINGS_H

#include <QWidget>

#include "generaldialog.h"
#include "usbmsSplash.h"

namespace Ui {
class KoboxSettingsWidget;
}

class KoboxSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit KoboxSettingsWidget(QWidget *parent = nullptr);
    ~KoboxSettingsWidget();

    bool not_user_change = false;
    void openSettingsRebootDialog();
    void openResetKoboxDialog();

private slots:
    void on_okBtn_clicked();
    void on_checkBox_toggled(bool checked);
    void on_spinBox_valueChanged(int arg1);
    void on_pushButton_clicked();
    void on_resetKoboxBtn_clicked();

private:
    Ui::KoboxSettingsWidget *ui;
    GeneralDialog *generalDialogWindow;
    USBMSSplashWidget *usbmsWindow;
};

#endif // KOBOXSETTINGS_H
