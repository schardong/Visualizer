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
        loadVolumeAction = new QAction("Load Volume Data", this);
        connect(loadVolumeAction, SIGNAL(triggered()), this, SLOT(loadVolume()));

        loadVertexBranchMapAction = new QAction("Load Vertex-branch map Data", this);
        connect(loadVertexBranchMapAction, SIGNAL(triggered()), this, SLOT(loadVertexBranchMap()));

        loadOpacityTFAction = new QAction("Load Opacity Transfer Function", this);
        connect(loadOpacityTFAction, SIGNAL(triggered()), this, SLOT(loadOpacityTransferFunction()));

        loadColorTFAction = new QAction("Load Color Transfer Function", this);
        connect(loadColorTFAction, SIGNAL(triggered()), this, SLOT(loadColorTransferFunction()));

        quitAction = new QAction("Quit", this);
        connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

        mipAction = new QAction("Maximum Intensity Projection", this);
        connect(mipAction, SIGNAL(triggered()), this, SLOT(modeMIP()));

        avgAction = new QAction("Average Intensity Projection", this);
        connect(avgAction, SIGNAL(triggered()), this, SLOT(modeAVG()));

        comAction = new QAction("Composition Projection", this);
        connect(comAction, SIGNAL(triggered()), this, SLOT(modeCOM()));

        buildContourTreeAction = new QAction("Build Contour Tree", this);
        connect(buildContourTreeAction, SIGNAL(triggered()), this, SLOT(buildContourTree()));

        fileMenu = new QMenu("File", this);
        fileMenu->addAction(loadVolumeAction);
        fileMenu->addAction(loadVertexBranchMapAction);
        fileMenu->addAction(loadOpacityTFAction);
        fileMenu->addAction(loadColorTFAction);
        fileMenu->addSeparator();
        fileMenu->addAction(quitAction);

        viewMenu = new QMenu("View", this);
        viewMenu->addAction(mipAction);
        viewMenu->addAction(avgAction);
        viewMenu->addAction(comAction);

        toolMenu = new QMenu("Tools", this);
        toolMenu->addAction(buildContourTreeAction);

        menuBar()->addMenu(fileMenu);
        menuBar()->addMenu(viewMenu);
        menuBar()->addMenu(toolMenu);

        resize(600, 600);

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
        QString path = QFileDialog::getOpenFileName(this, "Load Dataset", QDir::home().absolutePath());
        if(!path.isEmpty()) {
            view->loadVolume(path.toStdString());
        }

    }

    void MainWindow::loadVertexBranchMap()
    {
        Logger::getInstance()->warn("void MainWindow::loadVertexBranchMap is not fully implemented yet!");
//        QString path = QFileDialog::getOpenFileName(this, "Load Dataset", QDir::home().absolutePath());
//        if(!path.isEmpty()) {
//            view->loadVolume(path.toStdString());
//        }

    }

    void MainWindow::loadOpacityTransferFunction()
    {
        Logger::getInstance()->warn("void MainWindow::loadOpacityTransferFunction is not fully implemented yet!");
//        QString path = QFileDialog::getOpenFileName(this, "Load Transfer Function", QDir::home().absolutePath());
//        if(!path.isEmpty()) {
//            view->loadTransferFuncion(path.toStdString());
//        }
    }

    void MainWindow::loadColorTransferFunction()
    {
        Logger::getInstance()->warn("void MainWindow::loadColorTransferFunction is not fully implemented yet!");
//        QString path = QFileDialog::getOpenFileName(this, "Load Transfer Function", QDir::home().absolutePath());
//        if(!path.isEmpty()) {
//            view->loadTransferFuncion(path.toStdString());
//        }
    }

    void MainWindow::quit()
    {
        close();
    }

    void MainWindow::modeMIP()
    {
        view->setRayTransversalMode(MultiDimRenderer::RAY_TRANSVERSAL::MIP);
    }

    void MainWindow::modeAVG()
    {
        view->setRayTransversalMode(MultiDimRenderer::RAY_TRANSVERSAL::AVG);
    }

    void MainWindow::modeCOM()
    {
        view->setRayTransversalMode(MultiDimRenderer::RAY_TRANSVERSAL::COM);
    }

    void MainWindow::buildContourTree()
    {
        Logger::getInstance()->warn("void MainWindow::buildContourTree is not fully implemented yet!");
        view->buildContourTree();
    }

}
