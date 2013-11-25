#include "mainwindow.h"
#include "viewport.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>

namespace Ui
{


    MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent)
    {
        loadVolAction = new QAction("Load Volume Data", this);
        connect(loadVolAction, SIGNAL(triggered()), this, SLOT(loadVolume()));

        loadTFAction = new QAction("Load Transfer Function", this);
        connect(loadTFAction, SIGNAL(triggered()), this, SLOT(loadTransferFunction()));

        quitAction = new QAction("Quit", this);
        connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

        mipAction = new QAction("Maximum Intensity Projection", this);
        connect(mipAction, SIGNAL(triggered()), this, SLOT(modeMIP()));

        avgAction = new QAction("Average Intensity Projection", this);
        connect(avgAction, SIGNAL(triggered()), this, SLOT(modeAVG()));

        comAction = new QAction("Composition Projection", this);
        connect(comAction, SIGNAL(triggered()), this, SLOT(modeCOM()));

        fileMenu = new QMenu("File", this);
        fileMenu->addAction(loadVolAction);
        fileMenu->addAction(loadTFAction);
        fileMenu->addSeparator();
        fileMenu->addAction(quitAction);

        viewMenu = new QMenu("View", this);
        viewMenu->addAction(mipAction);
        viewMenu->addAction(avgAction);
        viewMenu->addAction(comAction);

        menuBar()->addMenu(fileMenu);
        menuBar()->addMenu(viewMenu);

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

    void MainWindow::modeMIP()
    {
        view->setRayTransversalMode(Renderer::RAY_TRANSVERSAL::MIP);
    }

    void MainWindow::modeAVG()
    {
        view->setRayTransversalMode(Renderer::RAY_TRANSVERSAL::AVG);
    }

    void MainWindow::modeCOM()
    {
        view->setRayTransversalMode(Renderer::RAY_TRANSVERSAL::COM);
    }

}
