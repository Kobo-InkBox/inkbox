#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>

#include "functions.h"

namespace Ui {
class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();
    global::wifi::wifiNetworkData mainData;
    QString currentlyConnectedNetwork;

signals:
    void showToastSignal(QString message);
    void refreshScreenSignal();

public slots:
    void applyVariables();
    void closeWrapper();
    void showToastSlot(QString message);
    void refreshScreenSlot();

private slots:
    void on_enterButton_clicked();

private:
    Ui::NetworkWidget *ui;
};

#endif // NETWORK_H
