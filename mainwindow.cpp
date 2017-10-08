#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mUi(new Ui::MainWindow),
    mWorld(new WorldFrame())
{
    mUi->setupUi(this);

    setCentralWidget(mWorld);

    mWorld->initRenderThread();
}

MainWindow::~MainWindow()
{
    delete mWorld;
    delete mUi;
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    mWorld->stopRenderThread();    // stop the thread befor exiting
    QMainWindow::closeEvent(evt);
}
