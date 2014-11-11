#include "mainwindow.h"
#include <QApplication>
#include "RTAudioDriver.hpp"
#include "engine.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    /*AbstractAudioDriver *drv = new RtAudioDriver();
    Engine *engine = new Engine(drv);

    drv->configure();
    drv->start();

    char input;
    std::cin >> input;

    drv->stop();

    delete drv;
    delete engine;*/

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
