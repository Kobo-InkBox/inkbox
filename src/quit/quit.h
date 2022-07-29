#ifndef QUIT_H
#define QUIT_H

#include <QWidget>

using namespace std;

namespace Ui {
class quit;
}

class quit : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit quit(QWidget *parent = nullptr);
    ~quit();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void on_backBtn_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::quit *ui;
};

#endif // QUIT_H
