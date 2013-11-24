#include "mainwindow.h"
#include "viewport.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>

namespace Ui
{

    const float MainWindow::STEP = 8;

    MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent)
    {
        loadVolAction = new QAction("Load Volume Data", this);
        connect(loadVolAction, SIGNAL(triggered()), this, SLOT(loadVolume()));

        loadTFAction = new QAction("Load Transfer Function", this);
        connect(loadTFAction, SIGNAL(triggered()), this, SLOT(loadTransferFunction()));

        quitAction = new QAction("Quit", this);
        connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

        fileMenu = new QMenu("File", this);
        fileMenu->addAction(loadVolAction);
        fileMenu->addAction(loadTFAction);
        fileMenu->addSeparator();
        fileMenu->addAction(quitAction);

        menuBar()->addMenu(fileMenu);

        resize(800, 600);

        QGLFormat f;
        f.setVersion(4, 2);
        f.setProfile(QGLFormat::CoreProfile);

        view = new Ui::Viewport(f, this);
        setCentralWidget(view);
    }

    MainWindow::~MainWindow()
    {
        delete view;
    }

    void MainWindow::loadVolume()
    {
        QString path = QFileDialog::getOpenFileName(this, "Load Volume", QDir::home().absolutePath());
        if(!path.isEmpty()) {
            view->loadVolume(path.toStdString());
        }

    }

    void MainWindow::loadTransferFunction()
    {
        QString path = QFileDialog::getOpenFileName(this, "Load Transfer Function", QDir::home().absolutePath());
        if(!path.isEmpty()) {
            view->loadTransferFuncion(path.toStdString());
        }
    }

    void MainWindow::quit()
    {
        close();
    }

//    void MainWindow::increaseSamples()
//    {
//        view->setNumSamples(view->getNumSamples() + STEP);
//    }

//    void MainWindow::decreaseSamples()
//    {
//        view->setNumSamples(view->getNumSamples() - STEP);
//    }

}
