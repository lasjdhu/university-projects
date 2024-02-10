/**
 * Painting and updating Game events on the screen as VIEW
 * @file GameWidget.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Widget.h"

const int TILE_SIZE = 50;

GameWidget::GameWidget(Game *game, QWidget *parent) :
    QWidget(parent),
    game(game)
{
    setFixedSize(game->get_width() * TILE_SIZE, game->get_height() * TILE_SIZE);
}

void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Draw map
    for (int y = 0; y < game->get_height(); y++) {
        for (int x = 0; x < game->get_width(); x++) {
            MapObject o = game->map->get_object(x, y);
            QRect rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            QPixmap pixmap;
            
            if (o != WALL) {
                pixmap = QPixmap("../src/images/grass.jpg");
                painter.drawPixmap(rect, pixmap);
            }

            switch (o) {
            case WALL:
                pixmap = QPixmap("../src/images/wall.jpg");
                painter.drawPixmap(rect, pixmap);
                break;
            case TARGET:
                pixmap = QPixmap("../src/images/flag.png");
                painter.drawPixmap(rect, pixmap);
                break;
            case KEY:
                if (!game->map->key->is_collected()) {
                    pixmap = QPixmap("../src/images/key.png");
                    painter.drawPixmap(rect, pixmap);
                }
                break;
            default:
                break;
            }
        }
    }

    // Draw characters
    QPixmap pacmanPixmap("../src/images/pacman.png");
    QPixmap ghostPixmapBlue("../src/images/ghostBlue.png");
    QPixmap ghostPixmapRed("../src/images/ghostRed.png");
    painter.drawPixmap(game->pacman->get_position_x() * TILE_SIZE,
                       game->pacman->get_position_y() * TILE_SIZE,
                       TILE_SIZE, TILE_SIZE, pacmanPixmap);
    for (int i = 0; i < game->get_ghost_count(); ++i) {
        if (i % 2 == 0) {
            painter.drawPixmap(game->ghosts[i]->get_position_x() * TILE_SIZE,
                               game->ghosts[i]->get_position_y() * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE, ghostPixmapBlue);
        } else {
            painter.drawPixmap(game->ghosts[i]->get_position_x() * TILE_SIZE,
                               game->ghosts[i]->get_position_y() * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE, ghostPixmapRed);
        }
    }
}

void GameWidget::updateGameState(Game *game) {
    update();
}
