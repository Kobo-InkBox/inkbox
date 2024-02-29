#ifndef HOURGLASSANIMATIONWIDGET_H
#define HOURGLASSANIMATIONWIDGET_H

#include <QWidget>

namespace Ui {
class hourglassAnimationWidget;
}

class hourglassAnimationWidget : public QWidget
{
    Q_OBJECT

public:
    QString className = this->metaObject()->className();
    explicit hourglassAnimationWidget(QWidget *parent = nullptr);
    ~hourglassAnimationWidget();
    int i;

private:
    Ui::hourglassAnimationWidget *ui;
};

#endif // HOURGLASSANIMATIONWIDGET_H
