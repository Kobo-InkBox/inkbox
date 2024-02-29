#ifndef USBMS_SPLASH_H
#define USBMS_SPLASH_H

#include <QWidget>

namespace Ui {
class USBMSSplashWidget;
}

class USBMSSplashWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit USBMSSplashWidget(QWidget *parent = nullptr);
    ~USBMSSplashWidget();
    float sW;
    float sH;
    QString massStorageModule;

    void usbmsLaunch();

private slots:
    void brightnessDown();
    void warmthDown();
    void quit_restart();
    void restartServices();

private:
    Ui::USBMSSplashWidget *ui;
};

#endif // USBMS_SPLASH_H
