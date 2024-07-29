#include "shork/cs/mainwindow.h"

#include <iostream>

#include <QApplication>

#include "shork/cs/aed_device.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    shork::cs::MainWindow w;
    w.show();
    return a.exec();
}
