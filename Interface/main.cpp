#include "mainwindow.h"
#include <QApplication>
#include <QGLFormat>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Ui::MainWindow w;
    w.setWindowTitle("3D Visualizer");
    w.show();

    return a.exec();
}
