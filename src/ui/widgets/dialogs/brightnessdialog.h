#ifndef BRIGHTNESSDIALOG_H
#define BRIGHTNESSDIALOG_H

#include <QDialog>

using namespace std;

namespace Ui {
class BrightnessDialog;
}

class BrightnessDialog : public QDialog
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    int oldValue;
    int oldWarmthValue;
    explicit BrightnessDialog(QWidget *parent = nullptr);
    ~BrightnessDialog();

private slots:
    void on_quitBtn_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_incBtn_clicked();
    void on_decBtn_clicked();
    void on_okBtn_clicked();
    void on_warmthSlider_valueChanged(int value);
    void on_warmthDecBtn_clicked();
    void on_warmthIncBtn_clicked();

private:
    Ui::BrightnessDialog *ui;
};

#endif // BRIGHTNESSDIALOG_H
