QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 c++17

QMAKE_CXXFLAGS += -Wno-unused-function -Wno-unused-parameter

# Thanks to https://github.com/mrbindraw/TestVersion/blob/master/TestVersion.pro
GIT_VERSION = $$system(git describe --always --tags)
GIT_COMMIT = $$system(git rev-parse HEAD)
COMMAND_REMOVE_MAKEFILES=$$quote(rm $$system_path($$OUT_PWD\Makefile*))
PRE_BUILD_TARGET = .dummyfile
updatemakefiles.target = $$PRE_BUILD_TARGET
updatemakefiles.commands = $$COMMAND_REMOVE_MAKEFILES
updatemakefiles.depends = FORCE
PRE_TARGETDEPS += $$PRE_BUILD_TARGET
QMAKE_EXTRA_TARGETS += updatemakefiles
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
DEFINES += GIT_COMMIT=\\\"$$GIT_COMMIT\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/platform/device.cpp \
    src/platform/device_desktop.cpp \
    src/platform/device_kindle_touch.cpp \
    src/platform/device_kobo_aura_edition2.cpp \
    src/platform/device_kobo_clarahd.cpp \
    src/platform/device_kobo_glo.cpp \
    src/platform/device_kobo_glohd.cpp \
    src/platform/device_kobo_libra.cpp \
    src/platform/device_kobo_mini.cpp \
    src/platform/device_kobo_nia.cpp \
    src/platform/device_kobo_touch.cpp \
    src/ui/apps/todo.cpp \
    src/audio/audiothread.cpp \
    src/ui/apps/apps.cpp \
    src/telemetry/telemetry.cpp \
    src/ui/splash/splashAlert.cpp \
    src/ui/ui.cpp \
    src/ui/widgets/dialogs/audio/audiodialog.cpp \
    src/ui/widgets/dialogs/audio/audiofile.cpp \
    src/ui/widgets/dialogs/audio/audiofilequeue.cpp \
    src/ui/widgets/dialogs/library/bookinfodialog.cpp \
    src/ui/widgets/dialogs/library/bookoptionsdialog.cpp \
    src/ui/widgets/dialogs/brightnessdialog.cpp \
    src/ui/apps/calendarapp.cpp \
    src/ui/widgets/dialogs/reader/highlightslistdialog.cpp \
    src/ui/widgets/dialogs/reader/textdialog.cpp \
    src/ui/widgets/dialogs/wifi/connectiondialog.cpp \
    src/ui/widgets/dialogs/wifi/network.cpp \
    src/ui/widgets/dialogs/wifi/wifilogger.cpp \
    src/ui/widgets/misc/egg.cpp \
    src/ui/widgets/reader/dictionarywidget.cpp \
    src/ui/crypto/encryptionmanager.cpp \
    src/ui/widgets/dialogs/generaldialog.cpp \
    src/ui/home/homepagewidget.cpp \
    src/ui/widgets/misc/hourglassanimationwidget.cpp \
    src/ui/widgets/dialogs/kobox/koboxappsdialog.cpp \
    src/ui/settings/koboxsettings.cpp \
    src/ui/onlineLibrary/librarywidget.cpp \
    src/ui/localLibrary/locallibrarywidget.cpp \
    src/main.cpp \
    src/ui/home/mainwindow.cpp \
    src/ui/otaUpdate/otamanager.cpp \
    src/ui/widgets/interfaceWidgets/qclickablelabel.cpp \
    src/ui/widgets/interfaceWidgets/qtooltiplabel.cpp \
    src/ui/quit/quit.cpp \
    src/ui/reader/reader.cpp \
    src/ui/apps/savedwords.cpp \
    src/ui/widgets/text/searchresultswidget.cpp \
    src/ui/settings/settings.cpp \
    src/ui/settings/settingschooser.cpp \
    src/ui/widgets/text/textwidget.cpp \
    src/ui/widgets/interfaceWidgets/toast.cpp \
    src/ui/splash/usbmsSplash.cpp \
    src/ui/apps/userapps.cpp \
    src/ui/widgets/virtualKeyboard/virtualkeyboard.cpp \
    src/ui/widgets/virtualKeyboard/virtualkeypad.cpp \
    src/ui/widgets/dialogs/wifi/wifidialog.cpp \
    src/ui/widgets/dialogs/powerDaemon/sleepdialog.cpp \
    src/powerDaemon/sleepthread.cpp \
    src/ui/settings/powerdaemonsettings.cpp

HEADERS += \
    src/audio/audiothread.h \
    src/platform/device.h \
    src/platform/device_desktop.h \
    src/platform/device_kindle_touch.h \
    src/platform/device_kobo_aura_edition2.h \
    src/platform/device_kobo_clarahd.h \
    src/platform/device_kobo_glo.h \
    src/platform/device_kobo_glohd.h \
    src/platform/device_kobo_libra.h \
    src/platform/device_kobo_mini.h \
    src/platform/device_kobo_nia.h \
    src/platform/device_kobo_touch.h \
    src/ui/apps/todo.h \
    src/ui/apps/apps.h \
    src/telemetry/telemetry.h \
    src/ui/splash/splashAlert.h \
    src/ui/ui.h \
    src/ui/widgets/dialogs/audio/audiodialog.h \
    src/ui/widgets/dialogs/audio/audiofile.h \
    src/ui/widgets/dialogs/audio/audiofilequeue.h \
    src/ui/widgets/dialogs/library/bookinfodialog.h \
    src/ui/widgets/dialogs/library/bookoptionsdialog.h \
    src/ui/widgets/dialogs/brightnessdialog.h \
    src/ui/apps/calendarapp.h \
    src/ui/widgets/dialogs/reader/highlightslistdialog.h \
    src/ui/widgets/dialogs/reader/textdialog.h \
    src/ui/widgets/dialogs/wifi/connectiondialog.h \
    src/ui/widgets/dialogs/wifi/network.h \
    src/ui/widgets/dialogs/wifi/wifilogger.h \
    src/ui/widgets/misc/egg.h \
    src/ui/widgets/reader/dictionarywidget.h \
    src/ui/crypto/encryptionmanager.h \
    src/functions.h \
    src/ui/widgets/dialogs/generaldialog.h \
    src/ui/home/homepagewidget.h \
    src/ui/widgets/misc/hourglassanimationwidget.h \
    src/ui/widgets/dialogs/kobox/koboxappsdialog.h \
    src/ui/settings/koboxsettings.h \
    src/ui/onlineLibrary/librarywidget.h \
    src/ui/localLibrary/locallibrarywidget.h \
    src/ui/home/mainwindow.h \
    src/ui/otaUpdate/otamanager.h \
    src/ui/widgets/interfaceWidgets/qclickablelabel.h \
    src/ui/widgets/interfaceWidgets/qtooltiplabel.h \
    src/ui/quit/quit.h \
    src/ui/reader/reader.h \
    src/ui/apps/savedwords.h \
    src/ui/widgets/text/searchresultswidget.h \
    src/ui/settings/settings.h \
    src/ui/settings/settingschooser.h \
    src/ui/widgets/text/textwidget.h \
    src/ui/widgets/interfaceWidgets/toast.h \
    src/ui/splash/usbmsSplash.h \
    src/ui/apps/userapps.h \
    src/ui/widgets/virtualKeyboard/virtualkeyboard.h \
    src/ui/widgets/virtualKeyboard/virtualkeypad.h \
    src/ui/widgets/dialogs/wifi/wifidialog.h \
    src/ui/widgets/dialogs/powerDaemon/sleepdialog.h \
    src/powerDaemon/sleepthread.h \
    src/ui/settings/powerdaemonsettings.h

FORMS += \
    src/ui/apps/todo.ui \
    src/ui/apps/apps.ui \
    src/ui/splash/splashAlert.ui \
    src/ui/widgets/dialogs/audio/audiodialog.ui \
    src/ui/widgets/dialogs/audio/audiofile.ui \
    src/ui/widgets/dialogs/audio/audiofilequeue.ui \
    src/ui/widgets/dialogs/library/bookinfodialog.ui \
    src/ui/widgets/dialogs/library/bookoptionsdialog.ui \
    src/ui/widgets/dialogs/brightnessdialog.ui \
    src/ui/apps/calendarapp.ui \
    src/ui/widgets/dialogs/reader/highlightslistdialog.ui \
    src/ui/widgets/dialogs/reader/textdialog.ui \
    src/ui/widgets/dialogs/wifi/connectiondialog.ui \
    src/ui/widgets/dialogs/wifi/network.ui \
    src/ui/widgets/dialogs/wifi/wifilogger.ui \
    src/ui/widgets/misc/egg.ui \
    src/ui/widgets/reader/dictionarywidget.ui \
    src/ui/crypto/encryptionmanager.ui \
    src/ui/widgets/dialogs/generaldialog.ui \
    src/ui/home/homepagewidget.ui \
    src/ui/widgets/misc/hourglassanimationwidget.ui \
    src/ui/widgets/dialogs/kobox/koboxappsdialog.ui \
    src/ui/settings/koboxsettings.ui \
    src/ui/onlineLibrary/librarywidget.ui \
    src/ui/localLibrary/locallibrarywidget.ui \
    src/ui/home/mainwindow.ui \
    src/ui/otaUpdate/otamanager.ui \
    src/ui/quit/quit.ui \
    src/ui/reader/reader.ui \
    src/ui/apps/savedwords.ui \
    src/ui/widgets/text/searchresultswidget.ui \
    src/ui/settings/settings.ui \
    src/ui/settings/settingschooser.ui \
    src/ui/widgets/text/textwidget.ui \
    src/ui/widgets/interfaceWidgets/toast.ui \
    src/ui/splash/usbmsSplash.ui \
    src/ui/apps/userapps.ui \
    src/ui/widgets/virtualKeyboard/virtualkeyboard.ui \
    src/ui/widgets/virtualKeyboard/virtualkeypad.ui \
    src/ui/widgets/dialogs/wifi/wifidialog.ui \
    src/ui/widgets/dialogs/powerDaemon/sleepdialog.ui \
    src/ui/settings/powerdaemonsettings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/ui/eink.qrc

INCLUDEPATH += $$system(find ./ -type d -print -path ./.git -prune | grep -v "./.git")
INCLUDEPATH += $$PWD/libs/libsndfile/include/
DEPENDPATH += $$PWD/libs/libsndfile/include/
LIBS += -L$$PWD/libs/prebuilt -lsndfile
