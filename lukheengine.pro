#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T23:28:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = lukheengine
TEMPLATE = lib

# ---- TARGET DEPENDENT ---
DEFINES += __RT_AUDIO__
win32 {
DEFINES += __NO_JACK__
 message(win32)
CONFIG += x86 console
QMAKE_CXXFLAGS += -D__WINDOWS_ASIO__ #-D__UNIX_JACK__ #RtAudio Flags
LIBS += -lWs2_32 -lOle32 #needed by rtaudio/asio
DEFINES += __WIN_32__
DEFINES -= UNICODE #RtAudio for Qt
}

INCLUDEPATH += D:/libs/rtaudio-4.1.1/ \
    D:/libs/rtaudio-4.1.1/include/


win32 {
HEADERS += D:/libs/rtaudio-4.1.1/RtAudio.h \
D:/libs/rtaudio-4.1.1/include/asio.h \
D:/libs/rtaudio-4.1.1/include/asiodrivers.h \
D:/libs/rtaudio-4.1.1/include/asiolist.h \
D:/libs/rtaudio-4.1.1/include/asiodrvr.h \
D:/libs/rtaudio-4.1.1/include/asiosys.h \
D:/libs/rtaudio-4.1.1/include/ginclude.h \
D:/libs/rtaudio-4.1.1/include/iasiodrv.h \
D:/libs/rtaudio-4.1.1/include/iasiothiscallresolver.h

LIBS += -L"D:/libs/rtaudio-4.1.1/lib" -lrtaudio_static
}



#--- LUKHE ENFINE SOURCES --
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

