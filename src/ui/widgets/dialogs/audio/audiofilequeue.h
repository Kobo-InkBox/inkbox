#ifndef AUDIOFILEQUEUE_H
#define AUDIOFILEQUEUE_H

#include <QWidget>
#include "functions.h"

namespace Ui {
class AudioFileQueueWidget;
}

class AudioFileQueueWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioFileQueueWidget(QWidget *parent = nullptr);
    ~AudioFileQueueWidget();
    QString className = this->metaObject()->className();
    bool isPlaying = false;

    void provideData(global::audio::musicFile fileProvided, bool gray);
    global::audio::musicFile file;

public slots:
    void die();

private:
    Ui::AudioFileQueueWidget *ui;

signals:
    void playFileChild(int itemInQueue);

private slots:
    void on_deleteBtn_clicked();
    void on_playBtn_clicked();
};

#endif // AUDIOFILEQUEUE_H
