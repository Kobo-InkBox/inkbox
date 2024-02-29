#include "apps.h"
#include "mainwindow.h"
#include "ui_apps.h"
#include "userapps.h"
#include "ui.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QProcess>
#include <QScreen>

AppsWidget::AppsWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AppsWidget) {
    Device *device = Device::getSingleton();

    ui->setupUi(this);
    ui->koboxAppsOpenButton->setProperty("type", "borderless");
    ui->calendarLaunchBtn->setProperty("type", "borderless");
    ui->savedWordsLaunchBtn->setProperty("type", "borderless");
    ui->calculatorLaunchBtn->setProperty("type", "borderless");
    ui->vncLaunchBtn->setProperty("type", "borderless");
    ui->reversiLaunchBtn->setProperty("type", "borderless");
    ui->g2048LaunchBtn->setProperty("type", "borderless");
    ui->todoLaunchBtn->setProperty("type", "borderless");

    ui->label->setStyleSheet("padding-top: 2px; padding-bottom: 5px");
    ui->koboxAppsOpenButton->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->calendarLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->savedWordsLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->calculatorLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->vncLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->reversiLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->g2048LaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
    ui->todoLaunchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");

    ui->noUserAppsAvailableLabel->hide();

    // Hiding KoBox apps button and label if X11 isn't enabled/wasn't started
    if (checkconfig("/external_root/boot/flags/X11_START") == false or checkconfig("/external_root/boot/flags/X11_STARTED") == false) {
        ui->label_5->hide();
        ui->koboxAppsOpenButton->hide();
        ui->label_5->deleteLater();
        ui->koboxAppsOpenButton->deleteLater();
    }
    // Hiding VNC viewer button and label if device is not rooted and doesn't have a working Wi-Fi adapter, or if X11 is not running
    if (checkconfig("/external_root/opt/root/rooted") == false) {
        if (device->supportsWifi() == false or checkconfig("/external_root/boot/flags/X11_START") == false or checkconfig("/external_root/boot/flags/X11_STARTED") == false) {
            ui->vncViewerLabel->hide();
            ui->vncLaunchBtn->hide();
            ui->vncViewerLabel->deleteLater();
            ui->vncLaunchBtn->deleteLater();
        }
    } else {
        if (checkconfig("/external_root/boot/flags/X11_START") == false or checkconfig("/external_root/boot/flags/X11_STARTED") == false) {
            ui->vncViewerLabel->hide();
            ui->vncLaunchBtn->hide();
            ui->vncViewerLabel->deleteLater();
            ui->vncLaunchBtn->deleteLater();
        }
    }

    ui->editUserAppsBtn->setProperty("type", "borderless");
    ui->editUserAppsBtn->setIcon(QIcon(":/resources/edit.png"));

    // Refresh because Qt shows the scrollbar for one second then hides it, leaving a mark on the eInk screen
    QTimer::singleShot(1750, this, SLOT(refreshScreenNative()));

    jsonParseSuccess = parseJson();
    showUserApps(false);

    UI::applyStyle(*this, "eink");
}

AppsWidget::~AppsWidget() {
    delete ui;
}

void AppsWidget::exitSlot() {
    AppsWidget::close();
}

void AppsWidget::on_savedWordsLaunchBtn_clicked() {
    log("Launching Saved Words app", className);
    savedWordsWindow = new SavedWordsAppWidget();
    savedWordsWindow->setAttribute(Qt::WA_DeleteOnClose);
    savedWordsWindow->showFullScreen();
}

void AppsWidget::on_calendarLaunchBtn_clicked() {
    log("Launching Calendar app", className);
    calendarWindow = new CalendarAppWidget();
    calendarWindow->setAttribute(Qt::WA_DeleteOnClose);
    calendarWindow->showFullScreen();
}

void AppsWidget::on_calculatorLaunchBtn_clicked() {
    log("Launching external Qalculate! app", className);
    QProcess process;
    process.startDetached("qalculate", QStringList());
    qApp->quit();
}

void AppsWidget::on_koboxAppsOpenButton_clicked() {
    log("Showing KoBox Apps Dialog", className);
    koboxAppsDialogWindow = new KoboxAppsDialog();
    connect(koboxAppsDialogWindow, SIGNAL(showToast(QString)), SLOT(showToastNative(QString)));
    koboxAppsDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    koboxAppsDialogWindow->show();
}

void AppsWidget::on_vncLaunchBtn_clicked() {
    log("Showing VNC dialog", className);
    global::keyboard::keyboardDialog = true;
    global::keyboard::vncDialog = true;
    global::keyboard::keyboardText = "";
    generalDialogWindow = new GeneralDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(generalDialogWindow, SIGNAL(refreshScreen()), SLOT(refreshScreenNative()));
    generalDialogWindow->show();
}

void AppsWidget::refreshScreenNative() {
    emit refreshScreen();
}

void AppsWidget::on_reversiLaunchBtn_clicked() {
    log("Launching external Reversi app", className);
    QProcess process;
    process.startDetached("qreversi", QStringList());
    qApp->quit();
}

void AppsWidget::on_g2048LaunchBtn_clicked() {
    log("Launching external 2048 app", className);
    QProcess process;
    process.startDetached("2048", QStringList());
    qApp->quit();
}

void AppsWidget::showToastNative(QString messageToDisplay) {
    emit showToast(messageToDisplay);
}

/*
 * This function opens and reads the main GUI user applications' JSON file, parses and verifies its contents to avoid issues in the future.
 */
bool AppsWidget::parseJson() {
    // If the path changes, it is also used statically in showUserApps()
    jsonFile.setFileName("/mnt/onboard/onboard/.apps/apps.json");
    bool jsonCheckSuccess = true;

    if (jsonFile.exists() == false) {
        log("GUI user applications' main JSON file is missing", className);
        jsonCheckSuccess = false;
    } else {
        jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QString fileRead = jsonFile.readAll();
        jsonFile.close();

        jsonDocument = QJsonDocument::fromJson(fileRead.toUtf8());
        if (jsonDocument.isNull() == true) {
            log("GUI user applications' main JSON file is invalid", className);
            jsonCheckSuccess = false;
        } else {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject["list"].isArray() == true) {
                QJsonArray jsonArray = jsonObject["list"].toArray();
                for (QJsonValueRef refJsonObject : jsonArray) {
                    if (refJsonObject.isObject()) {
                        QJsonObject jsonMainObject = refJsonObject.toObject();
                        if (jsonMainObject.size() == 9) {
                            if (!jsonMainObject["Name"].isString()) {
                                QString function = __func__;
                                log(function + ": Invalid 'Name' type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["Author"].isString()) {
                                QString function = __func__;
                                log(function + ": Invalid 'Author' type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["AuthorContact"].isString()) {
                                QString function = __func__;
                                log(function + ": Invalid 'AuthorContact' type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["Version"].isString()) {
                                QString function = __func__;
                                log(function + ": Invalid 'Version' type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["IconPath"].isString()) {
                                log("JSON: Invalid IconPath type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["ExecPath"].isString()) {
                                QString function = __func__;
                                log(function + ": Invalid 'ExecPath' type inside object", className);
                                jsonCheckSuccess = false;
                            } else {
                                if (jsonMainObject["ExecPath"].toString().contains("..")) {
                                    // Possible security risk
                                    showToastNative("ERROR: 'ExecPath' has invalid path");
                                    QString function = __func__;
                                    log(function + ": 'ExecPath' contains \"..\"", className);
                                    jsonCheckSuccess = false;
                                }
                            }
                            if (!jsonMainObject["Enabled"].isBool()) {
                                QString function = __func__;
                                log(function + ": Invalid 'Enabled' type inside object", className);
                                jsonCheckSuccess = false;
                            }
                            if (!jsonMainObject["SupportedDevices"].isArray()) {
                                QString function = __func__;
                                log(function + ": Invalid 'SupportedDevices' type inside object", className);
                                jsonCheckSuccess = false;

                            } else {
                                QJsonArray jsonArray = jsonMainObject["SupportedDevices"].toArray();
                                for (QJsonValueRef refJsonObject : jsonArray) {
                                    // https://doc.qt.io/qt-5/qjsonvalue.html#toInt
                                    if (!refJsonObject.isString()) {
                                        QString function = __func__;
                                        log(function + ": Array from 'RequiredFeatures' contains a wrong type", className);
                                        jsonCheckSuccess = false;
                                    }
                                }
                            }
                            if (!jsonMainObject["RequiredFeatures"].isArray()) {
                                QString function = __func__;
                                log(function + ": Invalid 'RequiredFeatures' type inside object", className);
                                jsonCheckSuccess = false;

                            } else {
                                QJsonArray jsonArray = jsonMainObject["RequiredFeatures"].toArray();
                                for (QJsonValueRef refJsonObject : jsonArray) {
                                    // https://doc.qt.io/qt-5/qjsonvalue.html#toInt
                                    if (!refJsonObject.isDouble()) {
                                        QString function = __func__;
                                        log(function + ": Array from 'RequiredFeatures' contains a wrong type", className);
                                        jsonCheckSuccess = false;
                                    }
                                }
                            }
                        } else {
                            QString function = __func__;
                            QString message = function + ": An object inside list array has too many items (or not enough): ";
                            message.append(QString::number(jsonMainObject.size()));
                            log(message, className);
                            jsonCheckSuccess = false;
                        }
                    } else {
                        QString function = __func__;
                        log(function + ": List array has an item of other type than object", className);
                        jsonCheckSuccess = false;
                    }
                }
            }
        }
    }
    return jsonCheckSuccess;
}

void AppsWidget::on_editUserAppsBtn_clicked() {
    if (userAppsSecondPage == false) {
        userAppsSecondPage = true;

        // Settings page
        ui->editUserAppsBtn->setIcon(QIcon(":/resources/save.png"));
        showUserApps(userAppsSecondPage);
        emit showUserAppsEdit(userAppsSecondPage);

        QTimer::singleShot(500, this, SLOT(refreshScreenNative()));
    } else {
        userAppsSecondPage = false;
        userAppsAvailable = false;

        // Launch page
        ui->editUserAppsBtn->setIcon(QIcon(":/resources/edit.png"));
        // It changed via updateJsonFileSlot, and now it writes it only once
        jsonFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
        jsonFile.write(jsonDocument.toJson());
        jsonFile.flush();
        jsonFile.close();

        showUserApps(userAppsSecondPage);
        emit showUserAppsEdit(userAppsSecondPage);

        QTimer::singleShot(500, this, SLOT(refreshScreenNative()));
    }
}

void AppsWidget::showUserApps(bool showDisabledJson) {
    emit clearAppsLayout();

    if (jsonParseSuccess == true) {
        QString function = __func__;
        log(function + ": Main user applications' JSON is valid", className);
        QJsonArray jsonListArray = jsonDocument.object()["list"].toArray();
        for (QJsonValueRef refJsonObject : jsonListArray) {
            QJsonObject appInfo = refJsonObject.toObject();
            if (appInfo["Enabled"].toBool() == true or showDisabledJson == true) {
                userAppsAvailable = true;
                UserAppsWidget *newUserApp = new UserAppsWidget;
                newUserApp->provideInfo(appInfo);
                connect(this, SIGNAL(clearAppsLayout()), newUserApp, SLOT(deleteLater()));
                connect(this, SIGNAL(showUserAppsEdit(bool)), newUserApp, SLOT(changePageEnabling(bool)));
                connect(this, SIGNAL(updateJsonFileSignal(QJsonDocument)), newUserApp, SLOT(updateJsonFileSlotUA(QJsonDocument)));
                connect(newUserApp, SIGNAL(updateJsonFileSignalUA(QJsonDocument)), this, SLOT(updateJsonFileSlot(QJsonDocument)));
                newUserApp->jsonDocument = jsonDocument;
                newUserApp->jsonFilePath = jsonFile.fileName();
                ui->verticalLayout_4->addWidget(newUserApp);
            }
        }

        if (userAppsAvailable == false) {
            ui->noUserAppsAvailableLabel->show();
        } else {
            ui->noUserAppsAvailableLabel->hide();
        }
    } else {
        if (QFile::exists("/mnt/onboard/onboard/.apps/apps.json")) {
            QString function = __func__;
            log(function + ": Main user applications' JSON file is invalid", className);
            QTimer::singleShot(500, this, SLOT(showFailedToParseMainUserAppsJsonFile()));
        } else {
            QString function = __func__;
            log(function + ": Main user applications' JSON file doesn't exist; assuming that no user applications are currently installed", className);
        }

        ui->editUserAppsBtn->hide();
        ui->label_6->hide();
        ui->line_2->hide();
        ui->line_3->hide();
        ui->editUserAppsBtn->hide();
        ui->label_6->deleteLater();
        ui->horizontalLayout->deleteLater();
        ui->line_2->deleteLater();
        ui->line_3->deleteLater();
    }
}

void AppsWidget::updateJsonFileSlot(QJsonDocument jsonDocumentFunc) {
    jsonDocument = jsonDocumentFunc;
    emit updateJsonFileSignal(jsonDocument);
}

void AppsWidget::showFailedToParseMainUserAppsJsonFile() {
    emit showToast("Failed to parse 'apps.json'");
}

void AppsWidget::on_todoLaunchBtn_clicked() {
    TodoAppWidget *todoWindow = new TodoAppWidget();
    QObject::connect(todoWindow, &TodoAppWidget::showToast, this, &AppsWidget::showToast);
    todoWindow->setAttribute(Qt::WA_DeleteOnClose);
    todoWindow->setGeometry(QRect(QPoint(0, 0), qApp->primaryScreen()->geometry().size()));
    todoWindow->show();
}
