#include "userapps.h"
#include "ui_userapps.h"
#include "mainwindow.h"
#include "generaldialog.h"

UserAppsWidget::UserAppsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserAppsWidget)
{
    ui->setupUi(this);
    ui->launchBtn->setProperty("type", "borderless");
    ui->launchBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");

    ui->launchBtn->setProperty("type", "borderless");

    ui->iconBtn->setProperty("type", "borderless");
    ui->iconBtn->setStyleSheet("QPushButton[type='borderless']:pressed { background: white; color: white; border: none; }");

    ui->infoBtn->setProperty("type", "borderless");
    ui->statusBtn->setProperty("type", "borderless");
    ui->statusBtn->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
}

UserAppsWidget::~UserAppsWidget()
{
    delete ui;
}

void UserAppsWidget::provideInfo(QJsonObject jsonInfo)
{
    QString name = jsonInfo["Name"].toString();
    appName = name; // It is for searching for json entry while disabling / enabling
    jsonObject = jsonInfo;
    // Limit name size to avoid breaking the GUI
    if(name.size() > 20) {
        // If someone wants to break the GUI, they will do it ¯\^-^/¯
        name.remove(16, 100);
    }
    ui->appNameLabel->setText(name);

    appDir.setPath("/mnt/onboard/onboard/.apps/" + name + "/" + name);

    QFile iconPath = QFile{appDir.path() + "/" + jsonInfo["IconPath"].toString()};
    if(iconPath.exists() == true) {
        QIcon appIcon = QIcon(iconPath.fileName());
        ui->iconBtn->setIconSize(QSize(40, 40));
        ui->iconBtn->setIcon(appIcon);
    }
    else {
        QString function = __func__;
        QString message = ": Warning: Icon not found: ";
        message.append(iconPath.fileName());
        log(function + message, className);
        ui->iconBtn->deleteLater();
        ui->gridLayout->deleteLater();
    }

    execPath.setFileName(jsonInfo["ExecPath"].toString());

    userAppEnabled = jsonInfo["Enabled"].toBool();
    if(userAppEnabled == true) {
        ui->statusBtn->setText("Disable");
    }
    else {
        ui->statusBtn->setText("Enable");
    }
}

// This function is needed when we dont want to repaint all widgets, but only change the the page (when no changes to the main JSON file were applied)
void UserAppsWidget::changePageEnabling(bool SecondPage)
{
    if(SecondPage == true){
        ui->stackedWidget->setCurrentIndex(1);
    }
    else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void UserAppsWidget::on_infoBtn_clicked()
{
    log("Launching user application information dialog", className);
    // https://stackoverflow.com/questions/28181627/how-to-convert-a-qjsonobject-to-qstring
    QString jsonStringParsed = parseJsonShow(jsonObject);
    global::text::textBrowserContents = jsonStringParsed;

    global::userApps::appInfoDialog = true;
    generalDialogWindow = new GeneralDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    generalDialogWindow->show();
}

void UserAppsWidget::on_statusBtn_clicked()
{
    ui->statusBtn->setEnabled(false);

    // Here the text on this button is used as a boolean; no need to create a new one
    // Disable and Enable
    if(userAppEnabled == false) {
        userAppEnabled = true;
        ui->statusBtn->setText("Disable");
    }
    else {
        userAppEnabled = false;
        ui->statusBtn->setText("Enable");
    }

    QJsonObject jsonRootObject = jsonDocument.object();
    QJsonArray jsonArrayList = jsonRootObject["list"].toArray();

    int arraySize = jsonArrayList.size();

    for(int i = 0; i < arraySize; i++) {
        QJsonObject jsonObject = jsonArrayList.at(i).toObject();
        QString entryName = jsonObject["Name"].toString();

        if(entryName == appName) {
            jsonObject.insert("Enabled", QJsonValue(userAppEnabled));

            QJsonArray sonArrayListNew = jsonDocument.object()["list"].toArray();
            sonArrayListNew.replace(i, jsonObject);

            jsonRootObject["list"] = sonArrayListNew;

            jsonDocument.setObject(jsonRootObject);
            emit updateJsonFileSignalUA(jsonDocument);
        }
    }
    ui->statusBtn->setEnabled(true);
}

void UserAppsWidget::updateJsonFileSlotUA(QJsonDocument jsonDocumentProvided)
{
    jsonDocument = jsonDocumentProvided;
}

void UserAppsWidget::on_launchBtn_clicked()
{
    // Some command to execute script or binary at "ExecPath"
    QJsonArray supportedDevices = jsonObject["SupportedDevices"].toArray();

    // This will work even if we are looking for 'n306' and there is a device named 'n306b' because QJsonArray::contains() works that way
    if(supportedDevices.contains("all") == false and supportedDevices.contains(Device::getSingleton()->getID()) == false) {
        log("Warning: User application '" + appName + "' does not support this device", className);
        global::userApps::appCompatibilityDialog = true;
        global::userApps::appCompatibilityText = "<font face='u001'>Your device is not compatible with this app.<br>Continue anyway</font><font face='Inter'>?</font>";
        generalDialogWindow = new GeneralDialog();
        generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);

        generalDialogWindow->exec();
    }
    else {
        global::userApps::launchApp = true;
    }

    if(manageRequiredFeatures() == true) {
        if(global::userApps::launchApp == true) {
            global::userApps::launchApp = false;
            QString message = "Launching user application at: '";
            message.append("[" + appDir.path() + "]" + execPath.fileName());
            message.append("'");
            log(message, className);
            // Tell the OS that we're not running anymore
            writeFile("/tmp/inkbox_running", "false");
            writeFile("/tmp/currentlyRunningUserApplication", appName);

            QProcess process;
            QStringList args;
            args << appDir.path() << execPath.fileName();
            process.startDetached("launch_user_application.sh", args);
            qApp->quit();
        }
    }
}

bool UserAppsWidget::manageRequiredFeatures()
{
    // This should be already set to 'true', but just in case
    global::userApps::appCompatibilityLastContinueStatus = true;
    QJsonArray jsonArray = jsonObject["RequiredFeatures"].toArray();
    for(QJsonValueRef refJsonObject: jsonArray) {
        bool launchDialog = false;
        int featureId = refJsonObject.toInt();
        // Wi-Fi connection required
        if(featureId == 0) {
            if(checkWifiState() != global::wifi::wifiState::configured) {
                global::userApps::appCompatibilityText = "<font face='u001'>This app needs a Wi-Fi connection, continue anyway</font><font face='Inter'>?</font>";
                launchDialog = true;
            }

        }
        // Rooted kernel required
        if(featureId == 1) {
            if(checkconfig("/external_root/opt/root/rooted") == false) {
                global::userApps::appCompatibilityText = "<font face='u001'>This app needs a rooted kernel, continue anyway</font><font face='Inter'>?</font>";
                launchDialog = true;
            }
        }
        // Pseudoterminal support (ID: 2) is managed by the 'gui_apps' service (https://github.com/Kobo-InkBox/rootfs/blob/master/etc/init.d/gui_apps)

        if(launchDialog == true) {
            global::userApps::appCompatibilityDialog = true;
            generalDialogWindow = new GeneralDialog();
            generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
            generalDialogWindow->exec();
        }

        if(global::userApps::appCompatibilityLastContinueStatus == false) {
            return false;
        }
    }
    return true;
}

QString UserAppsWidget::parseJsonShow(QJsonObject json)
{
    QString mainString;
    foreach(const QString& key, json.keys()) {
        QJsonValue value = json.value(key);

        QString appendString;

        appendString.append("<b>");
        appendString.append(key);
        appendString.append("</b>: ");

        if(value.isString()) {
            appendString.append(value.toString());
        }
        else if(value.isBool()) {
            appendString.append(QVariant(value.toBool()).toString());
        }
        else if(value.isArray()) {
            QJsonArray array = value.toArray();
            if(key == "RequiredFeatures") {
                bool foundRequiredFeature = false;
                for(QJsonValueRef ref: array) {
                    foundRequiredFeature = true;
                    int id = ref.toInt();
                    if(id == 0) {
                        appendString.append("Wi-Fi connection");
                    }
                    else if(id == 1) {
                        appendString.append("Rooted kernel");
                    }
                    else if(id == 2) {
                        appendString.append("Pseudoterminal support");
                    }
                    appendString.append(", ");
                }

                if(foundRequiredFeature == true) {
                    appendString.remove(appendString.size() - 2, 2);
                }
                else {
                    appendString.append("None");
                }
            }
            else if(key == "SupportedDevices") {
                for(QJsonValueRef ref: array) {
                    QString name = ref.toString();
                    appendString.append(name);
                    appendString.append(", ");
                }
                appendString.remove(appendString.size() - 2, 2);
            }

        }
        appendString.append("<br>");
        mainString.append(appendString);
    }

    return mainString;
}
