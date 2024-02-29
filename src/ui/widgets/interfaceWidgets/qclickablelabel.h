#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QLabel>
#include <QWidget>

class QClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit QClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QClickableLabel();
    long long timeAtClick;

signals:
    void clicked();
    void unclicked();
    void bookID(int id);
    void bookPath(QString path);
    void longPressInt(int id);
    void longPressString(QString bookPath);

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

};

#endif // CLICKABLELABEL_H
