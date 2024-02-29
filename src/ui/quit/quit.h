#ifndef QUIT_H
#define QUIT_H

#include <QWidget>

using namespace std;

namespace Ui {
class QuitWidget;
}

class QuitWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit QuitWidget(QWidget *parent = nullptr);
    ~QuitWidget();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void on_backBtn_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::QuitWidget *ui;
};

#endif // QUIT_H
