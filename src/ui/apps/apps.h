#ifndef APPS_H
#define APPS_H

#include <QWidget>
#include "savedwords.h"
#include "calendarapp.h"
#include "koboxappsdialog.h"
#include "generaldialog.h"
#include "todo.h"

#include <QJsonDocument>

namespace Ui {
class AppsWidget;
}

class AppsWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit AppsWidget(QWidget *parent = nullptr);
    ~AppsWidget();

private slots:
    void exitSlot();
    void on_savedWordsLaunchBtn_clicked();
    void on_calendarLaunchBtn_clicked();
    void on_calculatorLaunchBtn_clicked();
    void on_koboxAppsOpenButton_clicked();
    void on_vncLaunchBtn_clicked();
    void on_reversiLaunchBtn_clicked();
    void on_editUserAppsBtn_clicked();
    void refreshScreenNative();
    void on_g2048LaunchBtn_clicked();
    void showToastNative(QString messageToDisplay);
    bool parseJson();
    void showUserApps(bool showDisabledJson);
    void updateJsonFileSlot(QJsonDocument jsonDocument);
    void showFailedToParseMainUserAppsJsonFile();
    void on_todoLaunchBtn_clicked();

private:
    Ui::AppsWidget * ui;
    SavedWordsAppWidget * savedWordsWindow;
    CalendarAppWidget * calendarWindow;
    KoboxAppsDialog * koboxAppsDialogWindow;
    GeneralDialog * generalDialogWindow;
    TodoAppWidget * todoWindow;

    QFile jsonFile;
    QJsonDocument jsonDocument;
    bool jsonParseSuccess = false;
    bool userAppsSecondPage = false;
    bool userAppsAvailable = false;

signals:
    void refreshScreen();
    void showToast(QString messageToDisplay);
    void updateJsonFileSignal(QJsonDocument jsonDocument);
    void clearAppsLayout();
    void showUserAppsEdit(bool show);
};

#endif // APPS_H
