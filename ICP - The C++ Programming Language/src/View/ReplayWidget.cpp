/**
 * Painting and updating Game events from Replay as VIEW
 * @file ReplayWidget.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Widget.h"

const int TILE_SIZE = 50;

ReplayWidget::ReplayWidget(int rows, int cols, QString &map, QString &linesJoined, QWidget* parent) :
    QWidget(parent),
    rows(rows),
    cols(cols),
    map(map),
    linesJoined(linesJoined)
{
    setFixedSize(cols * TILE_SIZE, rows * TILE_SIZE);
}

void ReplayWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            QRect rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            QPixmap pixmap;
            QChar tile = map[y * cols + x];

            if (tile != 'X') {
                pixmap = QPixmap("../src/images/grass.jpg");
                painter.drawPixmap(rect, pixmap);
            }

            switch (tile.unicode()) {
            case 'X':
                pixmap = QPixmap("../src/images/wall.jpg");
                painter.drawPixmap(rect, pixmap);
                break;
            case 'T':
                pixmap = QPixmap("../src/images/flag.png");
                painter.drawPixmap(rect, pixmap);
                break;
            case 'K':
                if (!key_collected) {
                    pixmap = QPixmap("../src/images/key.png");
                    painter.drawPixmap(rect, pixmap);
                }
                break;
            default:
                break;
            }
        }
    }

    QPixmap pacmanPixmap("../src/images/pacman.png");
    QPixmap ghostPixmapBlue("../src/images/ghostBlue.png");
    QPixmap ghostPixmapRed("../src/images/ghostRed.png");
    if (p_x != -1) {
        painter.drawPixmap(p_x * TILE_SIZE, p_y * TILE_SIZE,
                       TILE_SIZE, TILE_SIZE, pacmanPixmap);
    }
    for (int i = 0; i < ghostPositions.size(); ++i) {
        int ghost_x = ghostPositions[i].x();
        int ghost_y = ghostPositions[i].y();

        if (i % 2 == 0) {
            painter.drawPixmap(ghost_x * TILE_SIZE, ghost_y * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE, ghostPixmapBlue);
        } else {
            painter.drawPixmap(ghost_x * TILE_SIZE, ghost_y * TILE_SIZE,
                               TILE_SIZE, TILE_SIZE, ghostPixmapRed);
        }
    }
}

void ReplayWidget::setIndex(int i) {
    QStringList lines = linesJoined.split("\n");
    key_collected = lines[i].split(',')[0].toInt();
    p_x = lines[i].split(',')[1].toInt();
    p_y = lines[i].split(',')[2].toInt();
    g = lines[i].split(',')[3].toInt();

    int index = 4;

    ghostPositions.clear();

    for (int cnt = 0; cnt < g; cnt++) {
        int ghost_x = lines[i].split(',')[index].toInt();
        int ghost_y = lines[i].split(',')[index + 1].toInt();
        ghostPositions.append({ghost_x, ghost_y});
        index += 2;
    }

    update();
}
