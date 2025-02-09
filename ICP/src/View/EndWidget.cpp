/**
 * Game Over or Win screen as VIEW
 * @file EndWidget.cpp
 * @authors Jakub Kratochvil (xkrato67), Dmitrii Ivanushkin (xivanu00)
 */

#include "Widget.h"

EndWidget::EndWidget(int mode, int steps, int tries, QString content, QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout(this);
    if (mode == 1) {
       label = new QLabel(QString("You won!\nSteps: %1\nTries: %2").arg(steps).arg(tries));
    } else {
        label = new QLabel(QString("Game Over\nSteps: %1\nTries: %2").arg(steps).arg(tries));
    }
    label->setStyleSheet("QLabel { color: white; }");
    layout->addWidget(label);
    layout->setAlignment(Qt::AlignCenter);

    exitButton = new QPushButton("Exit");
    exitButton->setStyleSheet("QPushButton { color: white; }");
    layout->addWidget(exitButton);

    QObject::connect(exitButton, &QPushButton::clicked, this, [this]() {
        hide();
        qApp->exit();
    });

    restartButton = new QPushButton("Restart");
    restartButton->setStyleSheet("QPushButton { color: white; }");
    layout->addWidget(restartButton);

    QObject::connect(restartButton, &QPushButton::clicked, this, [this, content]() {
        emit gameRestarted(content);
        hide();
    });

    saveButton = new QPushButton("Save this gameplay");
    saveButton->setStyleSheet("QPushButton { color: white; }");
    layout->addWidget(saveButton);

    labelSaved = new QLabel("Gameplay was saved");
    labelSaved->setStyleSheet("QLabel { color: white; }");

    QObject::connect(saveButton, &QPushButton::clicked, this, [this]() {
        emit saveGameplay();
        layout->addWidget(labelSaved);
    });

    setLayout(layout);
}

void EndWidget::updateContent(int mode, int steps, int tries) {
    if (mode == 1) {
        label->setText(QString("You won!\nSteps: %1\nTries: %2").arg(steps).arg(tries));
    } else {
        label->setText(QString("Game Over\nSteps: %1\nTries: %2").arg(steps).arg(tries));
    }
}
