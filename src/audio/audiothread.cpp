#include "audiothread.h"
#include "functions.h"

#include <QTimer>
// Socket things
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#include <QTimer>

AudioThread::AudioThread() {}

void AudioThread::start() {
    log("Audio thread monitor available", className);

    // QTimer doesn't work in such loops
    int count = 0;
    int delayMs = 100;
    int secondsToCount = 1000 / delayMs;
    int previousVolume = -1;
    while(true) {
        global::audio::audioMutex.lock();
        if(count == secondsToCount) {
            count = 0;
            if(monitorProgress == true) {
                audioProgress();
            }
        }
        foreach(global::audio::Action action, global::audio::currentAction) {
            // Order is important
            if(action == global::audio::Action::Stop) {
                // No need to call this before 'Play'
                log("Stop action received", className);
                sendInfo("pause:"); // Yea, only that
                global::audio::paused = true;
                global::audio::isSomethingCurrentlyPlaying = false;
                global::audio::progressSeconds = -5;
                monitorProgress = false;
            }
            if(action == global::audio::Action::Play) {
                log("'Play' action received", className);
                QString message = "play:\"";
                QString betterPath = global::audio::queue[global::audio::itemCurrentlyPlaying].path;
                betterPath.remove(0, 21); // Remove /mnt/onboard/onboard/
                log("The name of the song is: '" + global::audio::queue[global::audio::itemCurrentlyPlaying].name + "'", className);
                log("The path to be sent is: '" + betterPath + "'", className);

                message.append(betterPath);
                message.append('"');
                sendInfo(message);
                global::audio::paused = false;
                global::audio::isSomethingCurrentlyPlaying = true;
                global::audio::progressSeconds = -5;
                monitorProgress = true;
            }
            if(action == global::audio::Action::Pause) {
                log("'Pause' action received", className);
                QString message = "pause:";
                sendInfo(message);
                global::audio::paused = true;
                monitorProgress = false;
            }
            if(action == global::audio::Action::Continue) {
                log("'Continue' action received", className);
                QString message = "continue:";
                sendInfo(message);
                global::audio::paused = false;
                monitorProgress = true;
            }
        }
        global::audio::currentAction.clear();
        if(global::audio::volumeLevel != previousVolume) {
            previousVolume = global::audio::volumeLevel;
            log("'Set volume' action detected", className);
            QString message = "set_volume:" + QString::number(global::audio::volumeLevel);
            sendInfo(message);
        }
        global::audio::audioMutex.unlock();
        count = count + 1;
        QThread::msleep(delayMs);
    }
}

// I have no explanation as to why this needs to connect/disconnect every time.
// while (recv(client_sockfd, buffer_tmp, 1, 0) > 0) {
// ^ This line makes InkBox freeze
void AudioThread::sendInfo(QString message) {
    log("Sending message: *" + message + "*", className);
    // Send
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log("Error creating socket", className);
    }

    // Connect to the socket
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/dev/iaudio.socket");
    res = ::connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (res < 0) {
        log("Error connecting to socket", className);
    }

    log("Sending message: " + message, className);
    res = send(sockfd, message.toStdString().c_str(), strlen(message.toStdString().c_str()), 0);
    if (res < 0) {
        log("Error sending message to socket", className);
    }

    log("Message sent, exiting", className);
    close(sockfd);
}

// Mutex managed outside of the function
void AudioThread::audioProgress() {
    global::audio::progressSeconds = global::audio::progressSeconds + 1;
    // log("Progress, +1 sec: " + QString::number(global::audio::progressSeconds), className);
    if(global::audio::progressSeconds >= global::audio::queue[global::audio::itemCurrentlyPlaying].lengths) {
        if(global::audio::itemCurrentlyPlaying >= global::audio::queue.length()  - 1) {
            // It's the last item
            log("Last item: stopping playback", className);
            global::audio::isSomethingCurrentlyPlaying = false;
            global::audio::paused = true;
            monitorProgress = false;
            global::audio::progressSeconds = -5;
            global::audio::currentAction.append(global::audio::Action::Stop);
        }
        else {
            // It's not the last item, continuing
            log("Audio file changed", className);
            global::audio::itemCurrentlyPlaying = global::audio::itemCurrentlyPlaying + 1;
            global::audio::currentAction.append(global::audio::Action::Play);
            global::audio::songChanged = true;
        }
    }
}
