#include "userapp.h"
#include "ui_userapp.h"
#include "mainwindow.h"
#include "generaldialog.h"

userapp::userapp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userapp)
{
    ui->setupUi(this);
    ui->pushButtonLaunch->setProperty("type", "borderless");
    ui->pushButtonLaunch->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");

    ui->pushButtonIcon->setProperty("type", "borderless");

    ui->pushButtonAppInfo->setProperty("type", "borderless");
    ui->pushButton_2EnablingButton->setProperty("type", "borderless");
    ui->pushButton_2EnablingButton->setStyleSheet("background: lightGrey; font-size: 9pt; padding: 8px");
}

userapp::~userapp()
{
    delete ui;
}

void userapp::provideInfo(QJsonObject jsonInfo)
{
    QString name = jsonInfo["Name"].toString();
    appName = name; // Its for searching for json entry while disabling / enabling
    jsonObject = jsonInfo;
    // Limit name size to avoid breaking the gui
    if(name.size() > 20)
    {
        // If someone wants to break the gui, they will do it ¯\^-^/¯
        // Idk if this will work for every device
        name.remove(16, 100);
    }
    ui->labelAppName->setText(name);

    appDir.setPath("/mnt/onboard/onboard/.apps/" + name);

    QFile iconPath = QFile{appDir.path() + "/" + jsonInfo["IconPath"].toString()};
    if(iconPath.exists() == true)
    {
        QIcon appIcon = QIcon(iconPath.fileName());
        ui->pushButtonIcon->setIconSize(QSize(40, 40));
        ui->pushButtonIcon->setIcon(appIcon);
    } else {
        QString message = "Icon not found: ";
        message.append(iconPath.fileName());
        log(message, className);
    }

    execPath.setFileName(appDir.path() + "/" + jsonInfo["ExecPath"].toString());

    userAppEnabled = jsonInfo["Enabled"].toBool();
    if(userAppEnabled == true)
    {
        ui->pushButton_2EnablingButton->setText("Disable");
    } else {
        ui->pushButton_2EnablingButton->setText("Enable");
    }
}

// This function is needed when we dont want to repaint all widgets, but only change the the page ( when no changes to json were applied )
void userapp::changePageEnabling(bool SecondPage)
{
    if(SecondPage == true)
    {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void userapp::on_pushButtonAppInfo_clicked()
{
    // Show a big qdialog with the whole part json in it
    log("Launching json information dialog", className);
    global::text::textBrowserDialog = true;
    // https://stackoverflow.com/questions/28181627/how-to-convert-a-qjsonobject-to-qstring
    QJsonDocument doc(jsonObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    global::text::textBrowserContents = jsonString;

    generalDialogWindow = new generalDialog();
    generalDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(generalDialogWindow, SIGNAL(destroyed(QObject*)), SLOT(setupSearchDialog()));
    connect(generalDialogWindow, SIGNAL(refreshScreen()), SLOT(refreshScreen()));
    connect(generalDialogWindow, SIGNAL(showToast(QString)), SLOT(showToast(QString)));
    connect(generalDialogWindow, SIGNAL(closeIndefiniteToast()), SLOT(closeIndefiniteToast()));
    connect(generalDialogWindow, SIGNAL(openBookFile(QString, bool)), SLOT(openBookFile(QString, bool)));
    generalDialogWindow->show();
}

void userapp::on_pushButton_2EnablingButton_clicked()
{
    ui->pushButton_2EnablingButton->setEnabled(false);

    // Here the text on this button is used as a bool. No need to create a new one
    // Disable and Enable
    if(userAppEnabled == false)
    {
        userAppEnabled = true;
        ui->pushButton_2EnablingButton->setText("Disable");
    } else {
        userAppEnabled = false;
        ui->pushButton_2EnablingButton->setText("Enable");
    }

    // this looks the entire json for its entry and changes the Enabled bool. also checks if it actually changed
    // https://forum.qt.io/topic/100810/how-to-modify-data-and-save-to-json-file/4
    // https://stackoverflow.com/questions/69492325/update-value-in-qjsonarray-and-write-back-to-json-file-in-qt
    // https://stackoverflow.com/questions/19267335/qt-modifying-a-json-file
    // Resources for the future

    QJsonObject jsonRootObject = jsonDocument.object();
    QJsonArray jsonArrayList = jsonRootObject["list"].toArray();

    int arraySize = jsonArrayList.size();

    for(int i = 0; i < arraySize; i++)
    {
        QJsonObject jsonObject = jsonArrayList.at(i).toObject();
        QString entryName = jsonObject["Name"].toString();

        /*
        QString message = "JSON Searching for: ";
        message.append(appName);
        message.append(" Found: ");
        message.append(entryName);
        log(message, className);
        */

        if(entryName == appName)
        {
            jsonObject.insert("Enabled", QJsonValue(userAppEnabled));

            // Its accessing jsonDocument again becouse it could be changed via updateJsonFileSlot
            QJsonArray sonArrayListNew = jsonDocument.object()["list"].toArray();
            sonArrayListNew.replace(i, jsonObject);

            jsonRootObject["list"] = sonArrayListNew;

            jsonDocument.setObject(jsonRootObject);
            emit updateJsonFileSignalUA(jsonDocument);

            QFile jsonFile = jsonFilePath;
            jsonFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
            jsonFile.write(jsonDocument.toJson());
            jsonFile.flush();
            jsonFile.close();
        }
    }
    ui->pushButton_2EnablingButton->setEnabled(true);
}

void userapp::updateJsonFileSlotUA(QJsonDocument jsonDocumentProvided)
{
    jsonDocument = jsonDocumentProvided;
}

void userapp::on_pushButtonLaunch_clicked()
{
    // Some powerfull command to execute binary at "execPath"
    // For now this:
    QString message = "Launching user app at: ";
    message.append(execPath.fileName());
    log(message, className);

    QProcess process;
    process.startDetached(execPath.fileName(), QStringList());
    qApp->quit();
    // mount -o remount,suid /kobo/mnt/onboard/onboard

}
