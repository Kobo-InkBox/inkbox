#include "audiofile.h"
#include "ui_audiofile.h"
#include "functions.h"

AudioFileWidget::AudioFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioFileWidget)
{
    ui->setupUi(this);
    AudioFileWidget::setFont(QFont("u001"));
    ui->nameLabel->setFont(QFont("u001"));
    ui->timeLabel->setFont(QFont("u001"));

    ui->addBtn->setProperty("type", "borderless");

    ui->nameLabel->setWordWrap(true);
}

AudioFileWidget::~AudioFileWidget()
{
    delete ui;
}

void AudioFileWidget::provideData(global::audio::musicFile fileProvided) {
    file = fileProvided;
    ui->nameLabel->setText(file.name);
    ui->timeLabel->setText(file.length);
}

void AudioFileWidget::die() {
    this->deleteLater();
    this->close();
}

void AudioFileWidget::on_addBtn_clicked()
{
    ui->addBtn->setDisabled(true);
    ui->addBtn->setStyleSheet("background: black;");

    log("Adding item (audio file) to queue", className);
    global::audio::audioMutex.lock();
    global::audio::queue.append(file);
    if(global::audio::isSomethingCurrentlyPlaying == false) {
        log("Starting playback because nothing else is currently playing", className);
        global::audio::isSomethingCurrentlyPlaying = true;
        int tmpInt = global::audio::queue.length() - 1;
        global::audio::audioMutex.unlock();
        QTimer::singleShot(700, this, SLOT(enableButton()));
        emit playFileChild(tmpInt);
        return void();
    }
    global::audio::audioMutex.unlock();
    QTimer::singleShot(700, this, SLOT(enableButton()));
}

void AudioFileWidget::enableButton() {
    log("Enabling 'Back' button", className);
    ui->addBtn->setEnabled(true);
    ui->addBtn->setStyleSheet("background: white;");
    ui->addBtn->repaint();
}
