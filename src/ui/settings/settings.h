#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include <usbmsSplash.h>
#include "generaldialog.h"
#include "otamanager.h"
#include "toast.h"

using namespace std;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    int settings_page = 1;
    bool launch_sh = false;
    bool ui_enable_changed = false;
    bool ui_not_user_change = false;
    bool not_user_change = true;
    bool timezone_not_user_change = true;
    bool enableEncryptedStorageUserChange = false;

    float sW;
    float sH;
    float homeIconWidth;
    float homeIconHeight;

    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

private slots:
    void on_demoCheckBox_toggled(bool);
    void on_aboutBtn_clicked();
    void on_okBtn_clicked();
    void on_clockCheckBox_toggled(bool checked);
    void on_quoteCheckBox_toggled(bool checked);
    void on_requestLeaseBtn_clicked();
    void on_usbmsBtn_clicked();
    void on_updateBtn_clicked();
    void on_darkModeCheckBox_toggled(bool checked);
    void on_menuBarCheckBox_toggled(bool checked);
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_resetBtn_clicked();
    void on_setPasscodeBtn_clicked();
    void on_enableLockscreenCheckBox_toggled(bool checked);
    void on_showSystemInfoBtn_clicked();
    void on_readerScrollBarCheckBox_toggled(bool checked);
    void brightnessDown();
    void on_globalReadingSettingsCheckBox_toggled(bool checked);
    void on_checkOtaUpdateBtn_clicked();
    void openUpdateDialog();
    void launchOtaUpdater();
    void openUpdateDialogOTA(bool open);
    void showToastNative(QString messageToDisplay);
    void closeIndefiniteToastNative();
    void usbms_launch();
    void quit_restart();
    void on_enableEncryptedStorageCheckBox_toggled(bool checked);
    void disableStorageEncryption();
    void cancelDisableStorageEncryption();
    void on_repackBtn_clicked();
    void on_generateSystemReportBtn_clicked();
    void on_tzComboBox_currentTextChanged(const QString &arg1);
    void on_exportHighlightsBtn_clicked();
    void on_wordsNumberIncBtn_clicked();
    void on_wordsNumberDecBtn_clicked();
    void on_pageSizeHeightDecBtn_clicked();
    void on_pageSizeHeightIncBtn_clicked();
    void on_pageSizeWidthDecBtn_clicked();
    void on_pageSizeWidthIncBtn_clicked();
    void on_localLibraryShowFoldersCheckBox_toggled(bool checked);
    void on_usbmsDialogBox_clicked(bool checked);
    void on_autoCheckUpdatesBox_clicked(bool checked);
    void on_lockscreenBackgroundComboBox_currentTextChanged(const QString &arg1);
    void saveDeferredSettings();
    void on_settingsStackedWidget_currentChanged(int arg1);
    void on_readingSettingsBtn_clicked();
    void on_homeSettingsBtn_clicked();
    void on_librarySettingsBtn_clicked();
    void on_storageSettingsBtn_clicked();
    void on_systemSettingsBtn_clicked();
    void on_securitySettingsBtn_clicked();
    void on_headerBtn_clicked();
    void on_usbNetworkingCheckBox_toggled(bool checked);

signals:
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();

private:
    Ui::SettingsWidget * ui;
    USBMSSplashWidget * usbmsWindow;
    GeneralDialog * generalDialogWindow;
    OTAUpdateManagerWidget * otaManagerWindow;
    toast * toastWindow;

    int pageSizeHeightSaved;
    int pageSizeWidthSaved;
    int wordsNumberSaved;
};

#endif // SETTINGS_H
