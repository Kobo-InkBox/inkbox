#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings.h"
#include "settingschooser.h"
#include "apps.h"
#include "reader.h"
#include "quit.h"
#include "splashAlert.h"
#include "usbmsSplash.h"
#include "brightnessdialog.h"
#include "generaldialog.h"
#include "koboxsettings.h"
#include "koboxappsdialog.h"
#include "textwidget.h"
#include "virtualkeypad.h"
#include "toast.h"
#include "otamanager.h"
#include "librarywidget.h"
#include "locallibrarywidget.h"
#include "homepagewidget.h"
#include "telemetry.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    float stdIconWidth;
    float stdIconHeight;
    float brightnessIconWidth;
    float brightnessIconHeight;
    float homeIconWidth;
    float homeIconHeight;
    float wifiIconWidth;
    float wifiIconHeight;
    float sW;
    float sH;
    int truncateThreshold;

    bool existing_recent_books = false;
    bool reboot_after_update = false;
    bool resetFullWindowException;

    global::wifi::wifiState lastWifiState = global::wifi::wifiState::unknown;
    bool isConnecting = false;
    bool isReconnecting = false;

    int timerTime = 0;
    QString relative_path;
    QString usbmsStatus;

    void openLowBatteryDialog();
    void openCriticalBatteryAlertWindow();
    void openUsbmsDialog();
    void resetIcons();
    void setBatteryIcon();

    QString updatesCheckConfigPath = ".config/23-updates/check-updates";
    QString usbConfigPath = ".config/22-usb/show-dialog";

public slots:
    void showToast(QString messageToDisplay);
    void closeIndefiniteToast();

private slots:
    void on_settingsBtn_clicked();
    void on_appsBtn_clicked();
    void on_pushButton_clicked();
    void on_searchBtn_clicked();
    void on_quitBtn_clicked();
    void on_brightnessBtn_clicked();
    void openUpdateDialog();
    void setInitialBrightness();
    void on_homeBtn_clicked();
    void refreshScreen();
    void setupSearchDialog();
    void updateWifiAble();
    void on_wifiBtn_clicked();
    void updateWifiIcon();
    void hello(int testNumber);
    void openUpdateDialogOTA(bool open);
    void launchOtaUpdater();
    void openBookFile(QString book, bool relativePath);
    void openReaderFramework();
    void checkForUpdate();
    void checkForOtaUpdate();
    void openEncfsRepackDialog();
    void on_libraryButton_clicked();
    void resetWindow(bool resetStackedWidget);
    void resetFullWindow();
    void setupLocalLibraryWidget();
    void setupHomePageWidget();
    void launchOnlineLibrary();
    void on_audioBtn_clicked();
    void openTelemetryDialog();
    void openTelemetryMessageDialog();
    void sendDeviceInformationSlot(QString message);

private:
    Ui::MainWindow * ui;
    SettingsChooserWidget * settingsChooserWindow;
    AppsWidget * appsWindow;
    ReaderWidget * readerWindow;
    QuitWidget * quitWindow;
    SplashAlertWidget * alertWindow;
    USBMSSplashWidget * usbmsWindow;
    BrightnessDialog * brightnessDialogWindow;
    GeneralDialog * generalDialogWindow;
    KoboxSettingsWidget * koboxSettingsWindow;
    KoboxAppsDialog * koboxAppsDialogWindow;
    textwidget * textwidgetWindow;
    virtualkeypad * keypadWidget;
    toast * toastWindow;
    OTAUpdateManagerWidget * otaManagerWindow;
    LibraryWidget * libraryWidgetWindow;
    LocalLibraryWidget * localLibraryWidgetWindow;
    HomePageWidget * homePageWidgetWindow;
    Telemetry * telemetryInstance;
};

#endif // MAINWINDOW_H
