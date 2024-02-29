#include <QJsonDocument>
#include <QJsonObject>

#include "qclickablelabel.h"

QClickableLabel::QClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {}

QClickableLabel::~QClickableLabel() {}

void QClickableLabel::mousePressEvent(QMouseEvent * event) {
    timeAtClick = QDateTime::currentMSecsSinceEpoch();
    emit clicked();
    if(objectName() == "pageNumberLabel") {
        QClickableLabel::setStyleSheet("border-radius: 10px; padding-left: 10px; padding-right: 10px");
    }
    else {
        QClickableLabel::setStyleSheet("color: white; background-color: black; border-radius: 10px; padding: 10px");
    }
}

void QClickableLabel::mouseReleaseEvent(QMouseEvent * event) {
    if(QDateTime::currentMSecsSinceEpoch() >= timeAtClick + 500) {
        if(objectName().toInt()) {
            emit longPressInt(objectName().toInt());
        }
        else {
            emit longPressString(QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(objectName().toUtf8()))).object()["BookPath"].toString());
        }
    }
    else {
        emit unclicked();
        emit bookID(objectName().toInt());
        emit bookPath(QJsonDocument::fromJson(qUncompress(QByteArray::fromBase64(objectName().toUtf8()))).object()["BookPath"].toString());
    }
    if(objectName() == "pageNumberLabel") {
        QClickableLabel::setStyleSheet("border-radius: 10px; padding-left: 10px; padding-right: 10px");
    }
    else {
        QClickableLabel::setStyleSheet("color: black; background-color: white; border-radius: 10px; padding: 10px");
    }
}
