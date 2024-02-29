#ifndef VIRTUALKEYPAD_H
#define VIRTUALKEYPAD_H

#include <QWidget>
#include "functions.h"

namespace Ui {
class virtualkeypad;
}

class virtualkeypad : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit virtualkeypad(QWidget *parent = nullptr);
    ~virtualkeypad();

private slots:
    void on_n1_clicked();
    void on_clearBtn_clicked();
    void on_eraseBtn_clicked();
    void on_n2_clicked();
    void on_n3_clicked();
    void on_n4_clicked();
    void on_n5_clicked();
    void on_n6_clicked();
    void on_n7_clicked();
    void on_n8_clicked();
    void on_n9_clicked();
    void on_n0_clicked();

private:
    Ui::virtualkeypad *ui;
};

#endif // VIRTUALKEYPAD_H
