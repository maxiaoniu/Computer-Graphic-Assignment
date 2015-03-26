#include "mainwindow.h"
#include <QSurfaceFormat>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set OpenGL Version information
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 1);

    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    //w.resize(1024, 768);
    //w.setCentralWidget(widget);
    w.show();

    return a.exec();
}
