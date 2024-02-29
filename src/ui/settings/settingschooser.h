#ifndef SETTINGSCHOOSER_H
#define SETTINGSCHOOSER_H

#include <QWidget>

#include "settings.h"
#include "koboxsettings.h"

namespace Ui {
class SettingsChooserWidget;
}

class SettingsChooserWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit SettingsChooserWidget(QWidget *parent = nullptr);
    ~SettingsChooserWidget();

    float sW;
    float sH;
    float homeIconWidth;
    float homeIconHeight;

private slots:
    void on_inkboxSettingsBtn_clicked();
    void on_koboxSettingsBtn_clicked();
    void on_powerSettingsBtn_clicked();
    void showToastNative(QString messageToDisplay);
    void closeIndefiniteToastNative();

signals:
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();

private:
    Ui::SettingsChooserWidget *ui;
    SettingsWidget *settingsWindow;
    KoboxSettingsWidget *koboxSettingsWindow;
};

#endif // SETTINGSCHOOSER_H
