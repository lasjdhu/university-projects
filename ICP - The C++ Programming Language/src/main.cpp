/**
 * Main application initialization and show
 * @file main.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "View/mainwindow.h"
#include <QApplication>

/**
 * Main function
 * @param argc int
 * @param *argv[] char
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
