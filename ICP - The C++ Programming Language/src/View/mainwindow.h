/**
 * Main window header file
 * @file mainwindow.h
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../Controller/Controller.h"
#include "ui_mainwindow.h"
#include "Widget.h"
#include <QMainWindow>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * MainWindow class
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * MainWindow constructor
     * @param *parent pointer to a parent widget
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
