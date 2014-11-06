#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T23:28:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = lukheengine
TEMPLATE = lib

DEFINES += LUKHEENGINE_LIBRARY

INCLUDEPATH += .

SOURCES += \
    audio/components/track.cpp \
    audio/abstractaudiodriver.cpp \
    audio/engine.cpp \
    audio/components/audiocomponent.cpp

HEADERS += \
    const.hpp \
    audio/components/audiocomponent.hpp \
    audio/components/track.hpp \
    audio/abstractaudiodriver.hpp \
    audio/engine.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
