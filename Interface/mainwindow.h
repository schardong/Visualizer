#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "kernel.h"
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

        void modeMIP();
        void modeAVG();
        void modeCOM();

        void buildContourTree();
//        void increaseSamples();
//        void decreaseSamples();

    private:
        Ui::Viewport* view;

        QAction* loadVolAction;
        QAction* loadTFAction;
        QAction* quitAction;

        QAction* mipAction;
        QAction* avgAction;
        QAction* comAction;

        QAction* contourTreeAction;

        QMenu* fileMenu;
        QMenu* viewMenu;
        QMenu* toolMenu;

        static const float STEP;
    };

}

#endif // MAINWINDOW_H
