#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T23:28:47
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = lukheengine
TEMPLATE = app
CONFIG += x86_64 console noasio

# ---- TARGET DEPENDENT ---
DEFINES += __RT_AUDIO__
win32 {
DEFINES += __NO_JACK__
 message(win32)
asio{
DEFINE += __WINDOWS_ASIO__
QMAKE_CXXFLAGS += -D__WINDOWS_ASIO__ #-D__UNIX_JACK__ #RtAudio Flags
}
LIBS += -lWs2_32 -lOle32 -lAdvapi32 #needed by rtaudio/asio
DEFINES -= UNICODE #RtAudio for Qt

DEFINES += __WIN_32__

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
asio{
LIBS += -L"D:/libs/rtaudio-4.1.1/lib" -lrtaudio_asio_static
}
noasio{
LIBS += -L"D:/libs/rtaudio-4.1.1/lib" -lrtaudio_static
}
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
    platform/computer/audio/


SOURCES += \
    audio/components/track.cpp \
    audio/abstractaudiodriver.cpp \
    audio/engine.cpp \
    audio/components/audiocomponent.cpp \
    platform/computer/audio/RTAudioDriver.cpp \
    audio/components/randomcomponent.cpp

HEADERS += \
    const.hpp \
    audio/components/audiocomponent.hpp \
    audio/components/track.hpp \
    audio/abstractaudiodriver.hpp \
    audio/engine.hpp \
    platform/computer/audio/RTAudioDriver.hpp \
    audio/components/randomcomponent.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

