#include "ui.h"
#include "device.h"

#include <QFile>
#include <QThread>

UI::UI() {
}

void UI::applyStyle(QWidget &widget, QString styleName) {
    if(!styleName.startsWith(":") && !styleName.startsWith("/")) {
        styleName = Device::getSingleton()->getStyleSheetPath() + styleName;
    }
    if(!styleName.endsWith(".qss")) {
        styleName = styleName + "qss";
    }

    QFile stylesheetFile(styleName);
    stylesheetFile.open(QFile::ReadOnly);
    widget.setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();
}


void UI::transitionBrightness(int targetValue) {
    Device* device = Device::getSingleton();

    int currentBrightness = device->getBrightness();
    if(currentBrightness <= targetValue) {
        while(currentBrightness < targetValue) {
            currentBrightness++;
            device->setBrightness(currentBrightness);
            QThread::msleep(30);
        }
    } else {
        while(currentBrightness > targetValue) {
            currentBrightness--;
            device->setBrightness(currentBrightness);
            QThread::msleep(30);
        }
    }
}

void UI::transitionWarmth(int targetValue) {
    Device *device = Device::getSingleton();
    int currentWarmth = device->getWarmth();
    if (targetValue < currentWarmth) {
        while (targetValue < currentWarmth) {
            currentWarmth--;
            device->setWarmth(currentWarmth);
            QThread::msleep(30);
        }
    } else if (targetValue > currentWarmth) {
        while (targetValue > currentWarmth) {
            currentWarmth++;
            device->setWarmth(currentWarmth);
            QThread::msleep(30);
        }
    }
}
