#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QMessageBox>
#include <QDateTime>

#include "world.h"
#include "ui_mainwindow.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QMessageBox *mbox;

    int timerId;
    int tick = 0;
    bool is_paused;
    bool is_fast;
    int lastFpsUpdate = QDateTime::currentMSecsSinceEpoch();
    World world;

public:
    explicit MainWindow(QWidget *parent = 0) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        mbox(new QMessageBox),
        timerId(-1),
        is_paused(true),
        is_fast(false) {

        timerId = startTimer(1000 / 60);
        ui->setupUi(this);
        this->setFixedSize(this->geometry().width(), this->geometry().height());

        ui->txt_ticks->setText("0");
        connect(ui->btn_start_pause, SIGNAL(pressed()), this, SLOT(init_game()));
        connect(ui->btn_fast, SIGNAL(pressed()), this, SLOT(toggle_fast()));

        world = World();
        world.scale = (double)ui->viewport->height() / world.ysize;
        world.cameraPosition.x = ((double)ui->viewport->width() / world.scale - world.xsize) / 2;
    }

    ~MainWindow() {
        if (ui) delete ui;
    }


public slots:
    void init_game() {
        is_paused = !is_paused;
        if (is_paused)
            ui->btn_start_pause->setText("Продолжить");
        else
            ui->btn_start_pause->setText("Пауза");
    }

    void toggle_fast() {
        is_fast = !is_fast;
        if (is_fast)
            ui->btn_fast->setText("Медленно");
        else
            ui->btn_fast->setText("Быстро");
    }

public:
    void paintEvent(QPaintEvent*) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.translate(ui->viewport->x(), ui->viewport->y());
        painter.fillRect(ui->viewport->rect(), QBrush(Qt::white));
        painter.setClipRect(ui->viewport->rect());

        world.draw(painter);
    }

    void timerEvent(QTimerEvent *event) {
        if (event->timerId() == timerId && !is_paused) {
            auto lTime = QDateTime::currentMSecsSinceEpoch();
            int ticksNow = 0;
            while(QDateTime::currentMSecsSinceEpoch() < lTime + 1000 / 60 || ticksNow == 0) {
                // add random creature
                if (ui->spawnCells->isChecked() && tick % 20 == 0 && world.countCreatures() < 20)
                    world.addCreature(rand() % world.xsize, rand() % world.ysize, 800);
                // add meat
                if(rand() % 25 < ui->foodSpawn->value()) {
                    auto newCell = world.addCell(rand() % world.xsize, rand() % world.ysize, 200);
                    newCell->color = Qt::green;
                }
                // add virus
                if(rand() % 1000 < ui->virusSpawn->value())
                    world.addVirus(rand() % world.xsize, rand() % world.ysize, 800);

                world.update();
                tick++;
                ticksNow++;

                if(!is_fast)
                    break;
            }
            if ((int)QDateTime::currentMSecsSinceEpoch() > lastFpsUpdate + 400) {
                lastFpsUpdate = QDateTime::currentMSecsSinceEpoch();

                ui->fps->setText("fps: " + QString::number(ticksNow * 60));
                ui->statistics->setText(
                    "cells: " + QString::number(world.countCells()) + "\n" +
                    "joints: " + QString::number(world.countJoints()) + "\n" +
                    "creatures: " + QString::number(world.countCreatures()));
            }
            ui->txt_ticks->setText(QString::number(tick));
            this->update();
        }
    }

public:
    bool inViewport(int x, int y) {
        return x >= 0 && y >= 0 && x < ui->viewport->width() && y < ui->viewport->height();
    }

    void mouseMoveEvent(QMouseEvent *event) {
        int x = event->x() - ui->viewport->x();
        int y = event->y() - ui->viewport->y();

        if (inViewport(x, y))
            world.mouseMove(x, y);
    }

    void mousePressEvent(QMouseEvent *event) {
        int x = event->x() - ui->viewport->x();
        int y = event->y() - ui->viewport->y();

        if (inViewport(x, y))
            world.mousePress(x, y);
    }

    void mouseReleaseEvent(QMouseEvent *event) {
        int x = event->x() - ui->viewport->x();
        int y = event->y() - ui->viewport->y();

        if (inViewport(x, y))
            world.mouseRelease(x, y);
    }

    void keyPressEvent(QKeyEvent *event) {
        world.keyPress(event->key());
    }

    void wheelEvent(QWheelEvent *event) {
        world.wheel(event->delta() / 120.0);

        event->accept();
    }
};

#endif // MAINWINDOW_H
