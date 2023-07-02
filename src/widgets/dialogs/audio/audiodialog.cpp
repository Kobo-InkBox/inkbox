#include "audiodialog.h"
#include "ui_audiodialog.h"
#include "functions.h"
#include "audiofile.h"
#include "audiofilequeue.h"

#include <sndfile.h>
#include <unistd.h>
#include <math.h>

#include <QScreen>

audioDialog::audioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::audioDialog)
{
    ui->setupUi(this);

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


    int topButtonsSize = 55;
    int menuButtonsSize = 80;
    int playButtonsSize = 50;

    ui->minusBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->plusBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->exitBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});
    ui->refreshBtn->setIconSize(QSize{topButtonsSize,topButtonsSize});

    ui->nextBtn->setIconSize(QSize{playButtonsSize,playButtonsSize});
    ui->previousBtn->setIconSize(QSize{playButtonsSize,playButtonsSize});
    ui->playBtn->setIconSize(QSize{playButtonsSize,playButtonsSize});

    ui->libraryBtn->setIconSize(QSize{menuButtonsSize,menuButtonsSize});
    ui->queueBtn->setIconSize(QSize{menuButtonsSize,menuButtonsSize});

    ui->fileNameLabel->setWordWrap(true);

    // Default "page"
    ui->libraryBtn->setStyleSheet("background: grey;");
    ui->refreshBtn->setIcon(QIcon(":/resources/refresh-small.png"));

    if(global::audio::firstScan == true) {
        global::audio::firstScan = false;
        refreshFileList();
        log("Gathered file list", className);
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
        ui->playBtn->setIcon(QIcon("://resources/pause.png"));
    }
    global::audio::audioMutex.unlock();

    finishedStartingUp = true;
}

audioDialog::~audioDialog()
{
    delete ui;
}

void audioDialog::changeMenu() {
    log("Change menu requested", className);
    if(currentMenu != Queue) {
        currentMenu = Queue;
        emit deleteItself();
        ui->refreshBtn->show();
        ui->lineRefresh->show();
        ui->refreshBtn->setIcon(QIcon(":/resources/clean.png"));
        ui->libraryBtn->setStyleSheet("background: white;");
        ui->queueBtn->setStyleSheet("background: grey;");
        refreshAudioFileWidgetsQueue();
    }
    else if(currentMenu != Library){
        currentMenu = Library;
        emit deleteItself();
        ui->refreshBtn->hide();
        ui->lineRefresh->hide();
        ui->libraryBtn->setStyleSheet("background: grey;");
        ui->queueBtn->setStyleSheet("background: white;");
        refreshAudioFileWidgets();
    }
}

void audioDialog::on_libraryBtn_clicked()
{
    if(currentMenu != Library) {
        changeMenu();
    }
}

void audioDialog::on_queueBtn_clicked()
{
    if(currentMenu != Queue) {
        changeMenu();
    }
}

// Only on launching dialog
void audioDialog::refreshFileList() {
    log("Refreshing file list", className);
    if(QFile(".config/e-2-audio/path").exists() == false) {
        writeconfig(".config/e-2-audio/path", "/mnt/onboard/onboard/music/");
        log("Music config file doesn't exist, creating it", className);
    }
    // For example in this path: /mnt/onboard/onboard/music/ ( with / at the end )
    QString path = readFile(".config/e-2-audio/path").replace("\n", "");
    log("Path for audio files: " + path, className);
    QDir dir{path};
    dir.setNameFilters(QStringList("*.wav"));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();
    log("File count: " + QString::number(fileList.count()), className);
    global::audio::audioMutex.lock();
    for (int i=0; i < fileList.count(); i++)
    {
        log("Audio file: " + fileList[i], className);
        global::audio::musicFile newMusicFile;
        newMusicFile.path = path + fileList[i];
        QString tempName = fileList[i];
        tempName.chop(4); // File extension
        newMusicFile.name = tempName;
        log("File name length: " + QString::number(fileList[i].length()), className);
        log("File name: " + tempName, className);
        // Limiting file name length is managed in audiofile and audiofilequeue independently

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
        newMusicFile.length = QString::number(countMin) + ":" + QString::number(bareSeconds);
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

void audioDialog::refreshAudioFileWidgets() {
    global::audio::audioMutex.lock();
    emit deleteItself();
    // Doesn't work, freezes tha app...
    //QCoreApplication::processEvents();
    for(int i = 0; i < global::audio::fileList.size(); i++) {
        log("Adding new item number: " + QString::number(i), className);
        audiofile* newAudioFile = new audiofile(this);
        newAudioFile->provideData(global::audio::fileList[i]);
        QObject::connect(this, &audioDialog::deleteItself, newAudioFile, &audiofile::die);
        QObject::connect(newAudioFile, &audiofile::playFileChild, this, &audioDialog::playFile);
        ui->verticalLayout->addWidget(newAudioFile, Qt::AlignTop);
    }
    global::audio::audioMutex.unlock();
    this->adjustSize();
}

void audioDialog::refreshAudioFileWidgetsQueue() {
    global::audio::audioMutex.lock();
    emit deleteItself();
    // Doesn't work, freezes tha app...
    //QCoreApplication::processEvents();
    for(int i = global::audio::queue.size() - 1; i >= 0; i--) {
        log("Adding new item: " + QString::number(i), className);
        audiofilequeue* newAudioFileQueue = new audiofilequeue(this);
        bool grey = false;
        if(global::audio::itemCurrentlyPLaying == i) {
            grey = true;
        }
        newAudioFileQueue->provideData(global::audio::queue[i], grey);
        QObject::connect(this, &audioDialog::deleteItself, newAudioFileQueue, &audiofilequeue::die);
        QObject::connect(newAudioFileQueue, &audiofilequeue::playFileChild, this, &audioDialog::playFile);
        ui->verticalLayout->addWidget(newAudioFileQueue, Qt::AlignTop);
    }
    global::audio::audioMutex.unlock();
}

void audioDialog::on_refreshBtn_clicked()
{
    refreshAudioFileWidgetsQueue();
}

void audioDialog::playFile(int itemInQueue) {
    log("Called playFile", className);

    log("Calling stop to current actions to play a new file", className);
    global::audio::audioMutex.lock();
    global::audio::itemCurrentlyPLaying = itemInQueue;
    global::audio::songChanged = true;
    global::audio::currentAction.append(global::audio::Action::Play);
    global::audio::audioMutex.unlock();

    progress->stop();
    progressFuncManage();

    ui->playBtn->setIcon(QIcon("://resources/pause.png"));

    if(currentMenu == Queue) {
        refreshAudioFileWidgetsQueue();
    }
}

void audioDialog::progressFuncManage() {
    global::audio::audioMutex.lock();
    if(global::audio::isSomethingCurrentlyPlaying == true and global::audio::paused == false) {
        if(ui->progressSlider->maximum() != global::audio::queue[global::audio::itemCurrentlyPLaying].lengths) {
            ui->progressSlider->setMaximum(global::audio::queue[global::audio::itemCurrentlyPLaying].lengths);
        }
        //log("Changing slider position: " + QString::number(global::audio::progressSeconds), className);
        ui->progressSlider->setSliderPosition(global::audio::progressSeconds);
        if(global::audio::songChanged == true || finishedStartingUp == false) {
           QString currentName = global::audio::queue[global::audio::itemCurrentlyPLaying].name;
           if(currentName != ui->fileNameLabel->text()) {
               ui->fileNameLabel->setText(currentName);
               if(currentMenu == Queue) {
                   refreshAudioFileWidgetsQueue();
               }
               global::audio::songChanged = false;
           }
        }
        progress->singleShot(100, this, SLOT(progressFuncManage())); // For better accuracy, set 50
    } else {
        if(global::audio::isSomethingCurrentlyPlaying == false and global::audio::paused == true) {
           ui->progressSlider->setSliderPosition(0);
           ui->playBtn->setIcon(QIcon("://resources/play.png"));
           ui->fileNameLabel->setText("...");
           log("Exiting progress watcher", className);
        }
    }
    global::audio::audioMutex.unlock();
}

void audioDialog::on_progressSlider_sliderPressed()
{
    ui->progressSlider->releaseMouse();
}

void audioDialog::on_soundLevelSlider_valueChanged(int value)
{
    log("Setting volume level: " + QString::number(value), className);
    // It detects if volume changes
    if(finishedStartingUp == true) {
        global::audio::audioMutex.lock();
        global::audio::volumeLevel = value;
        global::audio::audioMutex.unlock();
    }
}

void audioDialog::on_plusBtn_clicked()
{
    ui->soundLevelSlider->setValue(ui->soundLevelSlider->value() + 1);
}

void audioDialog::on_minusBtn_clicked()
{
    ui->soundLevelSlider->setValue(ui->soundLevelSlider->value() - 1);
}

void audioDialog::on_playBtn_clicked()
{
    global::audio::audioMutex.lock();
    if(global::audio::isSomethingCurrentlyPlaying == true) {
        if(global::audio::paused == false) {
            global::audio::currentAction.append(global::audio::Action::Pause);
            ui->playBtn->setIcon(QIcon("://resources/play.png"));
        }
        else {
            global::audio::currentAction.append(global::audio::Action::Continue);
            ui->playBtn->setIcon(QIcon("://resources/pause.png"));
        }
    }
    else {
        // Risk: mutex still not unclocked
        if(global::audio::itemCurrentlyPLaying != -1) {
            int tmpInt = global::audio::itemCurrentlyPLaying;
            global::audio::audioMutex.unlock();
            playFile(tmpInt);
            return void();
        }
    }
    global::audio::audioMutex.unlock();

}

