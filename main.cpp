#include "mainwindow.h"
#include <QApplication>

#undef main

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.move(-8, -30);
    w.show();

    return a.exec();
}
