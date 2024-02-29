#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <QWidget>

namespace Ui {
class textwidget;
}

class textwidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit textwidget(QWidget *parent = nullptr);
    ~textwidget();

private:
    Ui::textwidget *ui;
};

#endif // TEXTWIDGET_H
