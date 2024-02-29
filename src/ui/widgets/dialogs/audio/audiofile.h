#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include "functions.h"

#include <QWidget>

namespace Ui {
class AudioFileWidget;
}

class AudioFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioFileWidget(QWidget *parent = nullptr);
    ~AudioFileWidget();
    QString className = this->metaObject()->className();
    void provideData(global::audio::musicFile fileProvided);
    global::audio::musicFile file;

public slots:
    void die();
    void enableButton();

private slots:

    void on_addBtn_clicked();

signals:
    void playFileChild(int itemInQueue);

private:
    Ui::AudioFileWidget *ui;
};

#endif // AUDIOFILE_H
