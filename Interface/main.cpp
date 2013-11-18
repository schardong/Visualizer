#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::getInstance()->setLogStream(&std::cout);
    Logger::getInstance()->log("Init");

    Ui::MainWindow w;
    w.setWindowTitle("3D Visualizer");
    w.show();

    return a.exec();
}
