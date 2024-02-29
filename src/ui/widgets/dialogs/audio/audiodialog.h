#ifndef AUDIODIALOG_H
#define AUDIODIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class AudioDialog;
}

class AudioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AudioDialog(QWidget *parent = nullptr);
    ~AudioDialog();
    QString className = this->metaObject()->className();

    enum audioMenu {
        Library,
        Queue,
    };

    bool currentMenu = Library;
    void changeMenu();
    void refreshFileList();
    void refreshAudioFileWidgets();
    void refreshAudioFileWidgetsQueue();
    QTimer* progress; // Needs to be stopped at exit
    bool finishedStartingUp = false;

public slots:
    void playFile(int itemInQueue); // Can be called from children
    void progressFuncManage();

private slots:
    void on_libraryBtn_clicked();
    void on_queueBtn_clicked();
    void on_refreshBtn_clicked();
    void on_progressSlider_sliderPressed();
    void on_soundLevelSlider_valueChanged(int value);
    void on_plusBtn_clicked();
    void on_minusBtn_clicked();
    void on_playBtn_clicked();
    void on_previousBtn_clicked();
    void on_nextBtn_clicked();
    void on_exitBtn_clicked();

signals:
    void deleteItself();

private:
    Ui::AudioDialog *ui;
};

#endif // AUDIODIALOG_H
