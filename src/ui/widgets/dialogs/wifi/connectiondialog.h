#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include "functions.h"

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit ConnectionDialog(QWidget *parent = nullptr);
    ~ConnectionDialog();
    global::wifi::wifiNetworkData connectedNetworkData;
    QString currentlyConnectedNetworkName;
    QFile passphraseDatabase = QFile("/mnt/onboard/.adds/inkbox/.config/17-wifi_connection_information/passphrases.json");

signals:
    void showToastSignal(QString message);
    void refreshScreenSignal();

public slots:
    void applyVariables();
    void showToastSlot(QString message);
    void refreshScreenSlot();

private slots:
    // We are opening/loading the JSON database many times, it might not be efficient, but:
    // 1. It's modular
    // 2. Those operations are rare
    QString searchDatabase(QString key);
    void writeToDatabase(QString name, QString passphrase);
    void removeFromDatabase(QString name);

    void finalConnectWait();
    bool checkIfWifiBusy();

    void on_cancelBtn_clicked();
    void on_passphraseTextEdit_selectionChanged();
    void on_passphraseTextEdit_cursorPositionChanged(int arg1, int arg2);
    void on_showPassphraseBtn_clicked();
    void on_connectBtn_clicked();

private:
    Ui::ConnectionDialog *ui;
    bool cursorPositionIgnore = false;
    bool showedPassphrase;
    QString savedPassphrase;
    int waitTry = 0;
    QString passphraseForReconnecting;
};

#endif // CONNECTIONDIALOG_H
