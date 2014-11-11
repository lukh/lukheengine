#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RTAudioDriver.hpp"
#include "engine.hpp"
#include "track.hpp"
#include "randomcomponent.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    AbstractAudioDriver *mDriver;
    Engine *mEngine;

    Track *mTrack;
    RandomComponent *mCompo;
};

#endif // MAINWINDOW_H
