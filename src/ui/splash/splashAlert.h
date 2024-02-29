#ifndef SPLASHALERT_H
#define SPLASHALERT_H

#include <QWidget>

namespace Ui {
class SplashAlertWidget;
}

using namespace std;

class SplashAlertWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit SplashAlertWidget(QWidget *parent = nullptr);
    ~SplashAlertWidget();
    bool signatureError = false;
    bool downgradeError = false;
    bool criticalBattery = false;
    void updateReset();

private slots:
    void on_continueBtn_clicked();
    void on_resetBtn_clicked();
    void on_continue2Btn_clicked();
    void quit();

private:
    Ui::SplashAlertWidget *ui;
};

#endif // SPLASHALERT_H
