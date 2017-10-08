#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "worldframe.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *evt);

private:
    Ui::MainWindow *mUi;
    WorldFrame* mWorld;
};

#endif // MAINWINDOW_H
