/**
 * Header file for both Interface and Game controllers
 * @file Controller.h
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../Model/Model.h"
#include "../View/mainwindow.h"
#include "../View/ui_mainwindow.h"
#include <QObject>
#include <QStatusBar>
#include <QtGui/QKeyEvent>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

class GameWidget;
class ReplayWidget;
class EndWidget;

/**
 * GameController class
 */
class GameController : public QObject{
    Q_OBJECT
public:
    /**
     * GameController constructor
     * @param *statusBar pointer to statusBar from MainWindow
     * @param *ui pointer to ui from MainWindow
     * @param *parent pointer to a parent widget
     */
    GameController(QStatusBar *statusBar, Ui::MainWindow *ui, QObject *parent);

    /**
     * Main thread with all processes for Game instance
     * @param &content content of map from text file
     */
    void runGame(QString &content);

    /**
     * Get actual state of game instance
     */
    Game* getGame();

    /**
     * All widgets manager
     */
    void initWidgets();

    /**
     * Create tmp file for log and return file
     */
    void createTmp();

    /**
     * Log game events
     * @param key_collected boolean for key
     * @param pacman_x position of pacman in x
     * @param pacman_y position of pacman in y
     * @param ghost number of ghost
     * @param ghost_x position of ghost in x (from vector)
     * @param ghost_y position of ghost in y (from vector)
     */
    void log(bool key_collected,
             int pacman_x,
             int pacman_y,
             int ghost,
             const std::vector<int>& ghost_x,
             const std::vector<int>& ghost_y);

    Direction temp_dir;
public slots:
    /**
     * Run new Game instance when file is loaded
     * @param content content of map from text file
     */
    void onFileLoaded(QString content);

    /**
     * Run new Game instance when game is restarted
     * @param content content of map from text file
     */
    void onGameRestarted(QString content);

    /**
     * Save tmp file with gameplay
     */
    void onSaveGameplay();

    /**
     * Replay your game
     */
    void replay();
private:
    QStatusBar *statusBar;
    QTimer timer;

    Game *game = nullptr;
    GameWidget *gameWidget = nullptr;
    ReplayWidget *replayWidget = nullptr;
    EndWidget *endWidget = nullptr;

    QLabel *stepsLabel = nullptr;
    QLabel *healthLabel = nullptr;

    QPushButton *backButton = nullptr;
    QPushButton *forwardButton = nullptr;
    QPushButton *exitButton = nullptr;
    QHBoxLayout *replayLayout = nullptr;
    QVBoxLayout *layout = nullptr;

    QString logFilename;
    QString startMap;

    int line_number = 0;

    int index = 0;
    int number_tries = 1;

    Ui::MainWindow *ui;

    /**
     * User input from keyboard to Pacman moving
     * @param *obj pointer to an object which receives the event
     * @param *event pointer to a key event
     */
    bool eventFilter(QObject *obj, QEvent *event);
};

/**
 * InterfaceController class
 */
class InterfaceController : public QObject{
    Q_OBJECT
public:
    /**
     * InterfaceController constructor
     * @param *statusBar pointer to statusBar from MainWindow
     * @param *ui pointer to ui from MainWindow
     * @param *parent pointer to a parent widget
     */
    InterfaceController(QStatusBar *statusBar, Ui::MainWindow *ui, QObject *parent);
signals:
    /**
     * signal from 'Load Game' button
     * @param content content of map from text file
     */
    void fileLoaded(QString content);
public slots:
    /**
     * Load map from text file
     */
    void loadFile();

    /**
     * Save current game state to file
     * @param game Game state - map
     */
    void saveFile(Game *game);

    /**
     * Display basic usage
     */
    void displayHelp();
private:
    QStatusBar *statusBar;
    Ui::MainWindow *ui;
};

#endif // GAMECONTROLLER_H
