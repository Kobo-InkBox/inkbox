#ifndef APPS_H
#define APPS_H

#include <QWidget>
#include <savedwords.h>
#include <calendarapp.h>
#include <koboxappsdialog.h>
#include <generaldialog.h>

#include <QJsonDocument>

namespace Ui {
class apps;
}

class apps : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit apps(QWidget *parent = nullptr);
    ~apps();

private slots:
    void on_scribbleLaunchBtn_clicked();
    void exitSlot();
    void on_lightmapsLaunchBtn_clicked();
    void on_savedWordsLaunchBtn_clicked();
    void on_calendarLaunchBtn_clicked();
    void on_calculatorLaunchBtn_clicked();
    void on_koboxAppsOpenButton_clicked();
    void on_vncLaunchBtn_clicked();
    void on_reversiLaunchBtn_clicked();
    void refreshScreenNative();
    void on_g2048LaunchBtn_clicked();
    void showToastNative(QString messageToDisplay);

    bool parseJson();

private:
    Ui::apps *ui;
    savedwords *savedWordsWindow;
    calendarApp *calendarWindow;
    koboxAppsDialog *koboxAppsDialogWindow;
    generalDialog *generalDialogWindow;
    QFile jsonFile;
    QJsonDocument jsonDocument;

signals:
    void refreshScreen();
    void showToast(QString messageToDisplay);
};

#endif // APPS_H
