#include "hourglassanimationwidget.h"
#include "ui_hourglassanimationwidget.h"

#include <QScreen>
#include <QThread>
#include <QTimer>
#include <QDebug>

hourglassAnimationWidget::hourglassAnimationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::hourglassAnimationWidget)
{
    ui->setupUi(this);

    // Getting the screen's size
    float sW = QGuiApplication::screens()[0]->size().width();
    float sH = QGuiApplication::screens()[0]->size().height();
    float stdIconWidth;
    float stdIconHeight;
    {
        stdIconWidth = sW / 2.5;
        stdIconHeight = sH / 2.5;
        QPixmap pixmap(":/resources/hourglass-top.png");
        QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->hourglassTopLabel->setPixmap(scaledPixmap);
    }
    {
        stdIconWidth = sW / 2.5;
        stdIconHeight = sH / 2.5;
        QPixmap pixmap(":/resources/hourglass-bottom.png");
        QPixmap scaledPixmap = pixmap.scaled(stdIconWidth, stdIconHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->hourglassBottomLabel->setPixmap(scaledPixmap);
    }

    i = 0;
    QTimer * t = new QTimer();
    t->setInterval(500);
    connect(t, &QTimer::timeout, [&]() {
        ui->stackedWidget->setCurrentIndex(i);
        if(i == 0) {
            i = 1;
        }
        else {
            i = 0;
        }
    } );
    t->start();
}

hourglassAnimationWidget::~hourglassAnimationWidget()
{
    delete ui;
}
