#include "audiofilequeue.h"
#include "ui_audiofilequeue.h"

AudioFileQueueWidget::AudioFileQueueWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioFileQueueWidget)
{
    ui->setupUi(this);
    AudioFileQueueWidget::setFont(QFont("u001"));
    ui->nameLabel->setFont(QFont("u001"));
    ui->timeLabel->setFont(QFont("u001"));

    ui->playBtn->setProperty("type", "borderless");
    ui->deleteBtn->setProperty("type", "borderless");

    ui->nameLabel->setWordWrap(true);
}

AudioFileQueueWidget::~AudioFileQueueWidget()
{
    delete ui;
}

void AudioFileQueueWidget::provideData(global::audio::musicFile fileProvided, bool gray) {
    file = fileProvided;
    ui->nameLabel->setText(file.name);
    ui->timeLabel->setText(file.length);
    if(gray == true) {
        log("Setting background gray", className);
        ui->deleteBtn->setStyleSheet("background: #aeadac;");
        ui->playBtn->setStyleSheet("background: #aeadac;");
        isPlaying = true;
    }
}

void AudioFileQueueWidget::die() {
    this->deleteLater();
    this->close();
}

void AudioFileQueueWidget::on_deleteBtn_clicked()
{
    int id = -1;
    log("global::audio::queue.size(): " + QString::number(global::audio::queue.size()), className);
    global::audio::audioMutex.lock();
    for(int i = 0; i < global::audio::queue.size(); i++) {
        if(file.id == global::audio::queue[i].id) {
            log("Found ID", className);
            id = i;
        }
    }

    log("File ID is: " + QString::number(id), className);
    global::audio::queue.remove(id);
    // TODO: Don't change song if the removed item is not playing
    // I will do it the moment I get annoyed about it
    log("After removing global::audio::queue.size(): " + QString::number(global::audio::queue.size()), className);
    if(id - 2 >= 0) {
        global::audio::audioMutex.unlock();
        emit playFileChild(id - 2);
        return void();
    } 
    else if(id < global::audio::queue.size()) {
        global::audio::audioMutex.unlock();
        emit playFileChild(id);
        return void();
    }
    else {
        global::audio::currentAction.append(global::audio::Action::Stop);
        global::audio::audioMutex.unlock();
        return void();
    }
    log("Something went wrong in deleting item that was in the query", className);
    global::audio::audioMutex.unlock();
}

void AudioFileQueueWidget::on_playBtn_clicked()
{
    global::audio::audioMutex.lock();
    for(int i = 0; i < global::audio::queue.size(); i++) {
        if(global::audio::queue[i].id == file.id) {
            global::audio::audioMutex.unlock();
            emit playFileChild(i);
            return void();
        }
    }
    global::audio::audioMutex.unlock();
    log("Something went really wrong", className);
}
