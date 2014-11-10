#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T23:28:47
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = lukheengine
TEMPLATE = app

# ---- TARGET DEPENDENT ---
DEFINES += __RT_AUDIO__
win32 {
DEFINES += __NO_JACK__
 message(win32)
CONFIG += x86_64 console
QMAKE_CXXFLAGS += -D__WINDOWS_ASIO__ #-D__UNIX_JACK__ #RtAudio Flags
LIBS += -lWs2_32 -lOle32 -lAdvapi32 #needed by rtaudio/asio
DEFINES += __WIN_32__
DEFINES -= UNICODE #RtAudio for Qt
}

INCLUDEPATH += D:/libs/rtaudio-4.1.1/ \
    D:/libs/rtaudio-4.1.1/include/


win32 {
HEADERS += D:/libs/rtaudio-4.1.1/RtAudio.h
#D:/libs/rtaudio-4.1.1/include/asio.h \
#D:/libs/rtaudio-4.1.1/include/asiodrivers.h \
#D:/libs/rtaudio-4.1.1/include/asiolist.h \
#D:/libs/rtaudio-4.1.1/include/asiodrvr.h \
#D:/libs/rtaudio-4.1.1/include/asiosys.h \
#D:/libs/rtaudio-4.1.1/include/ginclude.h \
#D:/libs/rtaudio-4.1.1/include/iasiodrv.h \
#D:/libs/rtaudio-4.1.1/include/iasiothiscallresolver.h

#SOURCES += D:/libs/rtaudio-4.1.1/include/asio.cpp \
#D:/libs/rtaudio-4.1.1/include/asiodrivers.cpp \
#D:/libs/rtaudio-4.1.1/include/asiolist.cpp \
#D:/libs/rtaudio-4.1.1/include/iasiothiscallresolver.cpp

LIBS += -L"D:/libs/rtaudio-4.1.1/lib" -lrtaudio_asio_static
}


#--- SANDBOX DEPENDENT ---
INCLUDEPATH += __sandbox__
SOURCES += __sandbox__/main.cpp \
        __sandbox__/mainwindow.cpp

HEADERS  += __sandbox__/mainwindow.h

FORMS    += __sandbox__/mainwindow.ui


#--- LUKHE ENGINE SOURCES --
INCLUDEPATH += . \
    audio/ \
    audio/components/ \
    platform/ \
    platform/audio/


SOURCES += \
    audio/components/track.cpp \
    audio/abstractaudiodriver.cpp \
    audio/engine.cpp \
    audio/components/audiocomponent.cpp \
    platform/audio/RTAudioDriver.cpp

HEADERS += \
    const.hpp \
    audio/components/audiocomponent.hpp \
    audio/components/track.hpp \
    audio/abstractaudiodriver.hpp \
    audio/engine.hpp \
    platform/audio/RTAudioDriver.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

