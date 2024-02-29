#ifndef CALENDARAPP_H
#define CALENDARAPP_H

#include <QWidget>

namespace Ui {
class CalendarAppWidget;
}

class CalendarAppWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit CalendarAppWidget(QWidget *parent = nullptr);
    ~CalendarAppWidget();

private slots:
    void on_backBtn_clicked();

private:
    Ui::CalendarAppWidget *ui;
};

#endif // CALENDARAPP_H
