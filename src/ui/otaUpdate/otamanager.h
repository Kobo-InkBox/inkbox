#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <QWidget>

namespace Ui {
class OTAUpdateManagerWidget;
}

class OTAUpdateManagerWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit OTAUpdateManagerWidget(QWidget *parent = nullptr);
    ~OTAUpdateManagerWidget();

private:
    Ui::OTAUpdateManagerWidget *ui;

signals:
    void canOtaUpdate(bool yesno);
    void downloadedOtaUpdate(bool wasDownloadSuccessful);
};

#endif // OTAMANAGER_H
