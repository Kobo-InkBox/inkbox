#ifndef QTOOLTIPLABEL_H
#define QTOOLTIPLABEL_H

#include <QLabel>
#include <QWidget>

class QToolTipLabel : public QLabel {
    Q_OBJECT

public:
    explicit QToolTipLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QToolTipLabel();

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

};

#endif // QTOOLTIPLABEL_H
