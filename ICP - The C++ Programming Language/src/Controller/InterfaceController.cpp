/**
 * Logic of application buttons as CONTROLLER
 * @file InterfaceController.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Controller.h"

InterfaceController::InterfaceController(QStatusBar *statusBar, Ui::MainWindow *ui, QObject *parent) :
    statusBar(statusBar), ui(ui), QObject(parent)
{}

void InterfaceController::loadFile() {
    // Get filename
    QString filename = QFileDialog::getOpenFileName(qobject_cast<QWidget*>(parent()),
                                                    "Open File",
                                                    "../examples/", "Text Files (*.txt)");
    if (filename.isEmpty()) {
        statusBar->showMessage("No file loaded");
        return;
    }

    // Open file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusBar->showMessage("Failed to load file");
        return;
    }

    // Read content, get strings and convert them to a byte array
    QTextStream in(&file);
    QString content = in.readAll();

    file.close();
    statusBar->showMessage(QString("File \"%1\" loaded").arg(filename));

    emit fileLoaded(content);
}

void InterfaceController::saveFile(Game *game) {
    // Get filename
    QString filename = QFileDialog::getSaveFileName(qobject_cast<QWidget*>(parent()),
                                                    "Save File",
                                                    "../examples/", "Text Files (*.txt)");
    if (filename.isEmpty()) {
        statusBar->showMessage("No file saved");
        return;
    }

    // Open file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        statusBar->showMessage("Failed to save file");
        return;
    }

    QString currentMap;

    int width = game->get_width();
    int height = game->get_height();

    currentMap += QString("%1 %2\n").arg(width).arg(height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == game->pacman->get_position_x() &&
                y == game->pacman->get_position_y())
            {
                currentMap += 'S';
            } else {
                bool is_ghost = false;
                for (int i = 0; i < game->get_ghost_count(); ++i) {
                    if (x == game->ghosts[i]->get_position_x() &&
                        y == game->ghosts[i]->get_position_y())
                    {
                        currentMap += 'G';
                        is_ghost = true;
                        break;
                    }
                }
                if (!is_ghost) {
                    MapObject o = game->map->get_object(x, y);
                    switch (o) {
                    case EMPTY:
                        currentMap += '.';
                        break;
                    case WALL:
                        currentMap += 'X';
                        break;
                    case KEY:
                        currentMap += 'K';
                        break;
                    case TARGET:
                        currentMap += 'T';
                        break;
                    default:
                        currentMap += '.';
                    }
                }
            }
        }
        currentMap += '\n';
    }

    // Write currentMap to a file
    QTextStream out(&file);
    out << currentMap;

    file.close();
    statusBar->showMessage(QString("File \"%1\" saved").arg(filename));
}

void InterfaceController::displayHelp() {
    QString message = "Pac-Man Game\
                       \
                       Your goal is to take a key from the exit point\
                       and find it untill ghosts can find you\
                       \
                       You can move Pac-Man using WASD\
                       To Load map in text format click 'Load' button\
                       To Save current game state to load and play it later click 'Save' button\
                       After the game you can save your gameplay and then watch it by clicking 'Replay' button";
    QMessageBox::information(qobject_cast<QWidget*>(parent()), "Help", message);
    statusBar->showMessage("Help is shown");
}
