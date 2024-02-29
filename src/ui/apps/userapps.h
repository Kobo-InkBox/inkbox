#ifndef USERAPPS_H
#define USERAPPS_H

#include "generaldialog.h"

#include <QWidget>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDir>
#include <QFile>

namespace Ui {
    class UserAppsWidget;
}

class UserAppsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit UserAppsWidget(QWidget * parent = nullptr);
    ~UserAppsWidget();
    QJsonDocument jsonDocument; // TODO: Optimize for RAM usage
    QJsonObject jsonObject; // Needed for 'App info' button
    QString jsonFilePath;

signals:
    // This is needed, because user application have their own JSON file. If the user changes it too fast it won't read it and overwrite previous changes
    void updateJsonFileSignalUA(QJsonDocument jsonDocument);


public slots:
    void provideInfo(QJsonObject jsonObject);
    void changePageEnabling(bool goThere);
    void updateJsonFileSlotUA(QJsonDocument jsonDocument);
    bool manageRequiredFeatures();
    QString parseJsonShow(QJsonObject json);

private slots:
    void on_infoBtn_clicked();
    void on_statusBtn_clicked();
    void on_launchBtn_clicked();

private:
    Ui::UserAppsWidget * ui;
    GeneralDialog * generalDialogWindow;

    QDir appDir;
    QFile execPath;
    bool userAppEnabled;
    QString appName;
};

#endif // USERAPPS_H
