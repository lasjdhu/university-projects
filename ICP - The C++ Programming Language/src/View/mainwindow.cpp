/**
 * Main window create and delete; Initialization of connects, ui and controllers as VIEW
 * @file mainwindow.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add label to the right
    QLabel* scoreLabel = new QLabel("");
    statusBar()->addPermanentWidget(scoreLabel);

    InterfaceController *interface = new InterfaceController(statusBar(), ui, this);
    GameController *game = new GameController(statusBar(), ui, this);

    QTimer::singleShot(0, interface, &InterfaceController::loadFile);
    statusBar()->showMessage("Please select your map");
    // Set listener for onFileLoaded signal
    connect(interface, &InterfaceController::fileLoaded, game, &GameController::onFileLoaded);

    // Load file
    connect(ui->actionLoad, &QAction::triggered, interface, &InterfaceController::loadFile);
    // Save file
    connect(ui->actionSave, &QAction::triggered, [=]() { interface->saveFile(game->getGame()); });
    // Load file
    connect(ui->actionReplay, &QAction::triggered, game, &GameController::replay);
    // Show help
    connect(ui->actionUsage, &QAction::triggered, interface, &InterfaceController::displayHelp);
}

MainWindow::~MainWindow() {
    delete ui;
}
