#include "audiodialog.h"
#include "ui_audiodialog.h"
#include "functions.h"
#include "audiofile.h"
#include "audiofilequeue.h"

#include <sndfile.h>
#include <unistd.h>
#include <math.h>

#include <QScreen>

AudioDialog::AudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioDialog)
{
    ui->setupUi(this);
    AudioDialog::setFont(QFont("u001"));
    ui->fileNameLabel->setFont(QFont("u001"));

    progress = new QTimer(this);

    // Size
    QRect screenGeometry = QGuiApplication::screens()[0]->geometry();
    this->setFixedWidth(screenGeometry.width() / 1.1);

    int halfOfHalfHeight = ((screenGeometry.height() / 2) / 2) / 2;
    int finalHeight = screenGeometry.height() - halfOfHalfHeight;

    this->setFixedHeight(finalHeight);

    // Centering dialog
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    int screenSizeProduct = screenGeometry.width() * screenGeometry.height();
    this->move(x, y);

    ui->minusBtn->setProperty("type", "borderless");
    ui->plusBtn->setProperty("type", "borderless");
    ui->exitBtn->setProperty("type", "borderless");
    ui->nextBtn->setProperty("type", "borderless");
    ui->previousBtn->setProperty("type", "borderless");
    ui->playBtn->setProperty("type", "borderless");
    ui->libraryBtn->setProperty("type", "borderless");
    ui->queueBtn->setProperty("type", "borderless");
    ui->refreshBtn->setProperty("type", "borderless");

    int topButtonsSize = screenSizeProduct / 14298.6;
    int menuButtonsSize = screenSizeProduct / 9830;
    int playBtnsSize = screenSizeProduct / 15728;

    ui->minusBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->plusBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->exitBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->refreshBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});

    ui->nextBtn->setIconSize(QSize{playBtnsSize,playBtnsSize});
    ui->previousBtn->setIconSize(QSize{playBtnsSize,playBtnsSize});
    ui->playBtn->setIconSize(QSize{playBtnsSize,playBtnsSize});

    ui->libraryBtn->setIconSize(QSize{menuButtonsSize,menuButtonsSize});
    ui->queueBtn->setIconSize(QSize{menuButtonsSize,menuButtonsSize});

    ui->fileNameLabel->setWordWrap(true);

    // Default "page"
    ui->libraryBtn->setStyleSheet("background: #aeadac;");
    ui->refreshBtn->setIcon(QIcon(":/resources/refresh-small.png"));
    ui->refreshBtn->hide();
    ui->lineRefresh->hide();

    if(global::audio::firstScan == true) {
        global::audio::firstScan = false;
        refreshFileList();
        log("Gathered files list", className);
    }

    refreshAudioFileWidgets();

    progressFuncManage();
    ui->progressSlider->setDisabled(true);

    int autoRepeatDelay = 400;
    int autoRepeatInterval = 20;

    ui->plusBtn->setAutoRepeat(true);
    ui->plusBtn->setAutoRepeatDelay(autoRepeatDelay);
    ui->plusBtn->setAutoRepeatInterval(autoRepeatInterval);

    ui->minusBtn->setAutoRepeat(true);
    ui->minusBtn->setAutoRepeatDelay(autoRepeatDelay);
    ui->minusBtn->setAutoRepeatInterval(autoRepeatInterval);

    global::audio::audioMutex.lock();
    ui->soundLevelSlider->setValue(global::audio::volumeLevel);

    if(global::audio::isSomethingCurrentlyPlaying == true) {
        ui->playBtn->setIcon(QIcon(":/resources/pause.png"));
    }
    global::audio::audioMutex.unlock();

    finishedStartingUp = true;
}

AudioDialog::~AudioDialog()
{
    delete ui;
}

void AudioDialog::changeMenu() {
    log("Menu change requested", className);
    if(currentMenu != Queue) {
        currentMenu = Queue;
        emit deleteItself();
        ui->refreshBtn->show();
        ui->lineRefresh->show();
        ui->refreshBtn->setIcon(QIcon(":/resources/clean.png"));
        ui->libraryBtn->setStyleSheet("background: white;");
        ui->queueBtn->setStyleSheet("background: #aeadac;");
        refreshAudioFileWidgetsQueue();
    }
    else if(currentMenu != Library){
        currentMenu = Library;
        emit deleteItself();
        ui->refreshBtn->hide();
        ui->lineRefresh->hide();
        ui->libraryBtn->setStyleSheet("background: #aeadac;");
        ui->queueBtn->setStyleSheet("background: white;");
        refreshAudioFileWidgets();
    }
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->minimum());
}

void AudioDialog::on_libraryBtn_clicked()
{
    if(currentMenu != Library) {
        changeMenu();
    }
}

void AudioDialog::on_queueBtn_clicked()
{
    if(currentMenu != Queue) {
        changeMenu();
    }
}

// Only on launching dialog
void AudioDialog::refreshFileList() {
    log("Refreshing file list", className);
    if(QFile(".config/e-2-audio/path").exists() == false) {
        writeFile(".config/e-2-audio/path", "/mnt/onboard/onboard/music/");
        log("Music config file doesn't exist, creating it", className);
    }
    // For example in this path: '/mnt/onboard/onboard/music/' (with '/' at the end)
    QString path = readFile(".config/e-2-audio/path").replace("\n", "");

    // Needed by GUI user apps
    if(path.isEmpty()) {
        path = "/mnt/onboard/onboard/music/";
    }
    
    log("Path for audio files: '" + path + "'", className);
    QDir dir{path};
    // Other file formats could be added, by building more libraries
    // https://github.com/arnavyc/sndfile-alsa-example/blob/main/src/sndfile-alsa.c
    // https://github.com/libsndfile/libsndfile
    // Is it easy to do? Yes. Does it take more space? Yes. Do I care? No, I have this fancy command instead:
    // for i in *; do ffmpeg -i "$i" "${i%.*}.wav"; done
    // To make the files smaller:
    // for i in *; do ffmpeg -i "$i" -sample_fmt s16 -ar 22000 "${i%.*}22k.wav"; done
    // (Szybet)
    dir.setNameFilters(QStringList("*.wav"));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();
    log("Files count: " + QString::number(fileList.count()), className);
    global::audio::audioMutex.lock();
    for (int i = 0; i < fileList.count(); i++)
    {
        log("Audio file: " + fileList[i], className);
        global::audio::musicFile newMusicFile;
        newMusicFile.path = path + fileList[i];
        log("File name path: " + newMusicFile.path, className);
        QString tempName = fileList[i];
        tempName.chop(4); // File extension
        newMusicFile.name = tempName;
        log("File name length: " + QString::number(fileList[i].length()), className);
        log("File name: " + tempName, className);

        // http://libsndfile.github.io/libsndfile/api#open_fd
        /*
            sf_count_t  frames ;
            int         samplerate ;
            int         channels ;
            int         format ;
            int         sections ;
            int         seekable ;
        */
        SF_INFO sfinfo;
        SNDFILE *wavFile = ::sf_open(newMusicFile.path.toStdString().c_str(), SFM_READ, &sfinfo);
        // http://soundfile.sapp.org/doc/WaveFormat/
        // https://stackoverflow.com/questions/53338925/get-the-audio-duration-using-libsndfile
        int bareSeconds = static_cast<double>(sfinfo.frames) / (sfinfo.samplerate);
        sf_close(wavFile);
        newMusicFile.lengths = bareSeconds;
        int countMin = 0;
        while(bareSeconds >= 60) {
            countMin = countMin + 1;
            bareSeconds = bareSeconds - 60;
        }
        QString min = QString::number(countMin);
        if(min.length() < 2) {
            min = '0' + min;
        }
        QString sec = QString::number(bareSeconds);
        if(sec.length() < 2) {
            sec = '0' + sec;
        }
        newMusicFile.length = min + ":" + sec;
        // To avoid shifting the line
        while(newMusicFile.length.length() < 4) {
            newMusicFile.length = newMusicFile.length + " ";
        }
        log("File length: " + newMusicFile.length, className);
        newMusicFile.id = i;
        global::audio::fileList.append(newMusicFile);
    }
    global::audio::audioMutex.unlock();
    this->adjustSize();
}

void AudioDialog::refreshAudioFileWidgets() {
    global::audio::audioMutex.lock();
    emit deleteItself();
    // Doesn't work, freezes the app...
    // QCoreApplication::processEvents();
    for(int i = 0; i < global::audio::fileList.size(); i++) {
        log("Adding new item number: " + QString::number(i), className);
        AudioFileWidget* newAudioFile = new AudioFileWidget(this);
        newAudioFile->provideData(global::audio::fileList[i]);
        QObject::connect(this, &AudioDialog::deleteItself, newAudioFile, &AudioFileWidget::die);
        QObject::connect(newAudioFile, &AudioFileWidget::playFileChild, this, &AudioDialog::playFile);
        ui->verticalLayout->addWidget(newAudioFile, Qt::AlignTop);
    }
    global::audio::audioMutex.unlock();
    this->adjustSize();
}

void AudioDialog::refreshAudioFileWidgetsQueue() {
    global::audio::audioMutex.lock();
    emit deleteItself();
    // Doesn't work, freezes the app...
    // QCoreApplication::processEvents();
    log("global::audio::itemCurrentlyPlaying: " + QString::number(global::audio::itemCurrentlyPlaying), className);
    log("global::audio::isSomethingCurrentlyPlaying: " + QString::number(global::audio::isSomethingCurrentlyPlaying), className);

    for(int i = 0; i < global::audio::queue.size(); i++) {
        log("Adding new item: " + QString::number(i), className);
        AudioFileQueueWidget* newAudioFileQueue = new AudioFileQueueWidget(this);
        bool gray = false;
        if(global::audio::isSomethingCurrentlyPlaying == true && global::audio::itemCurrentlyPlaying == i) {
            gray = true;
        }
        global::audio::queue[i].id = i; // Give them invidual ID once more, because files can repeat
        newAudioFileQueue->provideData(global::audio::queue[i], gray);
        QObject::connect(this, &AudioDialog::deleteItself, newAudioFileQueue, &AudioFileQueueWidget::die);
        QObject::connect(newAudioFileQueue, &AudioFileQueueWidget::playFileChild, this, &AudioDialog::playFile);
        ui->verticalLayout->addWidget(newAudioFileQueue, Qt::AlignTop);
    }
    global::audio::audioMutex.unlock();
}

void AudioDialog::on_refreshBtn_clicked()
{
    global::audio::audioMutex.lock();
    global::audio::currentAction.append(global::audio::Action::Stop);
    global::audio::isSomethingCurrentlyPlaying = false;
    global::audio::queue.clear();
    global::audio::audioMutex.unlock();
    refreshAudioFileWidgetsQueue();
}

void AudioDialog::playFile(int itemInQueue) {
    log("Called playFile", className);

    log("Calling stop to current actions to play a new file", className);
    global::audio::audioMutex.lock();
    // All those variables will be changed in audio thread too - but too late for progressFuncManage
    global::audio::itemCurrentlyPlaying = itemInQueue;
    global::audio::isSomethingCurrentlyPlaying = true;
    global::audio::paused = false;
    global::audio::songChanged = true;
    global::audio::currentAction.append(global::audio::Action::Play);
    global::audio::audioMutex.unlock();

    progress->stop();
    progressFuncManage();

    ui->playBtn->setIcon(QIcon(":/resources/pause.png"));

    if(currentMenu == Queue) {
        refreshAudioFileWidgetsQueue();
    }
}

void AudioDialog::progressFuncManage() {
    global::audio::audioMutex.lock();
    // log("Called progress watcher", className);

    bool requestWatcher = false;
    bool requestQueueRefresh = false;

    if(global::audio::isSomethingCurrentlyPlaying == true) {
        if(ui->progressSlider->maximum() != global::audio::queue[global::audio::itemCurrentlyPlaying].lengths) {
            ui->progressSlider->setMaximum(global::audio::queue[global::audio::itemCurrentlyPlaying].lengths);
        }
        // log("Changing slider position: " + QString::number(global::audio::progressSeconds), className);
        ui->progressSlider->setSliderPosition(global::audio::progressSeconds);
        if(global::audio::songChanged == true || finishedStartingUp == false) {
           log("global::audio::queue.size(): " + QString::number(global::audio::queue.size()), className);
           log("global::audio::itemCurrentlyPlaying: " + QString::number(global::audio::itemCurrentlyPlaying), className);
           log("Setting song name...", className);

           QString currentName = global::audio::queue[global::audio::itemCurrentlyPlaying].name;
           // log("New name: " + currentName, className);
           ui->fileNameLabel->setText(currentName);
           if(currentMenu == Queue) {
               requestQueueRefresh = true;
           }
           global::audio::songChanged = false;
        }
        if(global::audio::paused == false) {
           requestWatcher = true;
        }
    } 
    else {
       ui->progressSlider->setSliderPosition(0);
       ui->playBtn->setIcon(QIcon(":/resources/play.png"));
       ui->fileNameLabel->setText("...");
       if(currentMenu == Queue) {
           requestQueueRefresh = true;
       }
       // log("Exiting progress watcher", className);
    }
    global::audio::audioMutex.unlock();

    if(requestQueueRefresh == true) {
       refreshAudioFileWidgetsQueue();
    }

    if(requestWatcher == true) {
       // log("Launching another watcher...", className);
       progress->singleShot(450, this, SLOT(progressFuncManage())); // For better accuracy, set 50
    }
}

void AudioDialog::on_progressSlider_sliderPressed()
{
    ui->progressSlider->releaseMouse();
}

void AudioDialog::on_soundLevelSlider_valueChanged(int value)
{
    log("Setting volume level: " + QString::number(value), className);
    // It detects if volume changes
    if(finishedStartingUp == true) {
        global::audio::audioMutex.lock();
        global::audio::volumeLevel = value;
        global::audio::audioMutex.unlock();
    }
}

void AudioDialog::on_plusBtn_clicked()
{
    ui->soundLevelSlider->setValue(ui->soundLevelSlider->value() + 1);
}

void AudioDialog::on_minusBtn_clicked()
{
    ui->soundLevelSlider->setValue(ui->soundLevelSlider->value() - 1);
}

void AudioDialog::on_playBtn_clicked()
{
    global::audio::audioMutex.lock();
    if(global::audio::isSomethingCurrentlyPlaying == true) {
        if(global::audio::paused == false) {
            global::audio::paused = true;
            global::audio::currentAction.append(global::audio::Action::Pause);
            ui->playBtn->setIcon(QIcon(":/resources/play.png"));
        }
        else {
            global::audio::paused = false;
            global::audio::currentAction.append(global::audio::Action::Continue);
            ui->playBtn->setIcon(QIcon(":/resources/pause.png"));
            global::audio::audioMutex.unlock();
            progressFuncManage();
            return void();
        }
    }
    else if(global::audio::queue.size() - 1 >= global::audio::itemCurrentlyPlaying && global::audio::itemCurrentlyPlaying > 0) {
        int tmpInt = global::audio::itemCurrentlyPlaying;
        global::audio::audioMutex.unlock();
        playFile(tmpInt);
        ui->playBtn->setIcon(QIcon(":/resources/pause.png"));
        return void();
    }
    else {
        ui->playBtn->setIcon(QIcon(":/resources/stop.png"));
    }
    global::audio::audioMutex.unlock();
}

void AudioDialog::on_previousBtn_clicked()
{
    global::audio::audioMutex.lock();
    int item = global::audio::itemCurrentlyPlaying;
    item = item - 1;
    if(item >= 0 && global::audio::queue.size() >= 1) {
        global::audio::audioMutex.unlock();
        playFile(item);
        ui->previousBtn->setIcon(QIcon(":/resources/previous.png"));
        ui->nextBtn->setIcon(QIcon(":/resources/next.png"));
        return void();
    } else {
        ui->previousBtn->setIcon(QIcon(":/resources/stop.png"));
    }
    global::audio::audioMutex.unlock();
}

void AudioDialog::on_nextBtn_clicked()
{
    global::audio::audioMutex.lock();
    int item = global::audio::itemCurrentlyPlaying;
    item = item + 1;
    if(item >= 0 && item < global::audio::queue.size()) {
        global::audio::audioMutex.unlock();
        playFile(item);
        ui->previousBtn->setIcon(QIcon(":/resources/previous.png"));
        ui->nextBtn->setIcon(QIcon(":/resources/next.png"));
        return void();
    } else {
        ui->nextBtn->setIcon(QIcon(":/resources/stop.png"));
    }
    global::audio::audioMutex.unlock();
}

void AudioDialog::on_exitBtn_clicked()
{
    progress->stop();
    progress->deleteLater();

    // Make sure the mutex is unlocked
    global::audio::audioMutex.tryLock();
    global::audio::audioMutex.unlock();

    this->deleteLater();
    this->close();
}
