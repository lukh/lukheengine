#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),

    mDriver((AbstractAudioDriver*)NULLPTR),
    mEngine((Engine *)NULLPTR),
    mTrack((Track*)NULLPTR),
    mCompo((RandomComponent*)NULLPTR)
{
    ui->setupUi(this);

    mDriver = new RtAudioDriver();
    mEngine = new Engine(mDriver);

    mTrack = new Track(0, 0);
    mEngine->setComponent(0, mTrack);

    mCompo = new RandomComponent(0, 0);
    mTrack->setComponent(0, mCompo);

    mDriver->configure();
    mDriver->start();
}

MainWindow::~MainWindow()
{
    mDriver->stop();

    delete mTrack;
    delete mDriver;
    delete mEngine;

    delete ui;
}
