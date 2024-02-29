#ifndef ENCRYPTIONMANAGER_H
#define ENCRYPTIONMANAGER_H

#include <QWidget>
#include "generaldialog.h"
#include "toast.h"
#include "hourglassanimationwidget.h"
#include "splashAlert.h"

namespace Ui {
class CryptoManagerWidget;
}

class CryptoManagerWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit CryptoManagerWidget(QWidget *parent = nullptr);
    ~CryptoManagerWidget();
    bool setupExitWidgetRan = false;
    int setupPassphraseDialogMode;
    bool setupMessageBoxRan = false;
    int passphraseTries;

private slots:
    void on_setupContinueBtn_clicked();
    void on_setupAbortBtn_clicked();
    void quit_restart();
    void refreshScreen();
    void showToast(QString messageToDisplay);
    void setupEncryptedStorage();
    void mkEncfsDirs();
    void on_exitSuccessBtn_clicked();
    void setupExitWidget(bool exitStatus);
    void on_failureContinueBtn_clicked();
    void setupPassphraseDialog();
    void unlockEncryptedStorage();
    void setupFailedAuthenticationMessageBox();
    void on_acceptBtn_clicked();
    void on_usbmsBtn_clicked();
    void repackEncryptedStorage();

private:
    Ui::CryptoManagerWidget *ui;
    GeneralDialog * generalDialogWindow;
    toast * toastWindow;
    hourglassAnimationWidget * hourglassAnimationWidgetWindow;
    SplashAlertWidget * alertWindow;
    USBMSSplashWidget *usbmsWindow;
};

#endif // ENCRYPTIONMANAGER_H
