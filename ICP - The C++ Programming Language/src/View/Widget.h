/**
 * Widget header file
 * @file GameWidget.h
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "../Controller/Controller.h"
#include <QtWidgets>

/**
 * GameWidget class
 */
class GameWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * GameWidget constructor
     * @param *game pointer to an object of Game class
     * @param *parent pointer to a parent widget
     */
    GameWidget(Game *game, QWidget *parent = nullptr);

    /**
     * Update game on the screen
     * @param game pointer to an object of Game class
     */
    void updateGameState(Game *game);
protected:
    /**
     * Paint rectangles and load assets to a widget
     * @param *event QPaintEvent
     */
    void paintEvent(QPaintEvent *event) override;
private:
    Game *game;
};

/**
 * ReplayWidget class
 */
class ReplayWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * ReplayWidget constructor
     * @param rows
     * @param cols
     * @param map game map
     * @param linesJoined events in time
     * @param *parent pointer to a parent widget
     */
    ReplayWidget(int rows, int cols, QString &map, QString &linesJoined, QWidget *parent = nullptr);

    /**
     * Set index for line
     * @param i index of line
     */
    void setIndex(int i);
protected:
    /**
     * Paint rectangles and load assets to a widget
     * @param *event QPaintEvent
     */
    void paintEvent(QPaintEvent *event) override;
private:
    int rows;
    int cols;

    bool key_collected = 0;
    int p_x = -1;
    int p_y = -1;
    int g = -1;

    QList<QPoint> ghostPositions;
    QString map;
    QString linesJoined;
};

/**
 * EndWidget class
 */
class EndWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * EndWidget constructor
     * @param mode Game Over (0) or Win (1)
     * @param steps count of steps
     * @param tries count of tries
     * @param content current map for restart
     * @param *parent pointer to a parent widget
     */
    EndWidget(int mode, int steps, int tries, QString content, QWidget *parent = nullptr);

    /**
     * Update widget
     * @param mode Game Over (0) or Win (1)
     * @param steps count of steps
     * @param tries count of tries
     */
    void updateContent(int mode, int steps, int tries);
signals:
    /**
     * signal from 'Restart' button
     * @param content content of map from text file
     */
    void gameRestarted(QString content);

    /**
     * signal from 'Save this gameplay' button
     */
    void saveGameplay();
private:
    QVBoxLayout *layout;
    QLabel *label;
    QLabel *labelSaved;
    QPushButton *exitButton;
    QPushButton *restartButton;
    QPushButton *saveButton;
};

#endif // WIDGET_H
