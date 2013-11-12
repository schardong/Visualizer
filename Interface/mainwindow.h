#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GL3/gl3w.h"
#include "viewport.h"

#include <QMainWindow>

namespace Ui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setViewport(Ui::Viewport*);

private slots:
    void loadVolume();
    void loadTransferFunction();
    void quit();

private:
    Ui::Viewport* view;

    QAction* loadVolAction;
    QAction* loadTFAction;
    QAction* quitAction;

    QMenu* fileMenu;
};

}

#endif // MAINWINDOW_H
