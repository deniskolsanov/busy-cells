#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QMessageBox>
#include <QDateTime>

#include "ui_mainwindow.h"

#include "world.h"
#include "world2.h"
#include "test.h"


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
    World2 world;

    int targetFps = 8;
    long long nextUpdate = 0;

    //Test test;

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

        world.scale = int((double)ui->viewport->height() / world.ysize);
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

        //test.test(painter);
    }

    void timerEvent(QTimerEvent *event) {
        if (event->timerId() == timerId && !is_paused) {
            auto lTime = QDateTime::currentMSecsSinceEpoch();
            int ticksNow = 0;
            //while(QDateTime::currentMSecsSinceEpoch() < lTime + 1000 / targetFps/* || ticksNow == 0*/) {
            while(QDateTime::currentMSecsSinceEpoch() >= nextUpdate || (is_fast && QDateTime::currentMSecsSinceEpoch() < lTime + 1000 / 60) ) {
                // add random creature

                //if (ui->spawnCells->isChecked() && tick % 20 == 0 && world.countCreatures() < 40)
                //    world.addCreature(rand() % world.xsize, rand() % world.ysize, 800);

                // add meat
                if(rand() % 25 < ui->foodSpawn->value()) {
                    //auto newCell = world.physics.addCell(rand() % world.xsize, rand() % world.ysize, 100);
                    //auto newCell = world.physics.addCell(rand() % world.xsize, world.ysize * 3 / 4, rand() % 200 + 20);
                    //newCell->color = QColor(185, 120, 90);
                }
                // add virus
                //if(rand() % 1000 < ui->virusSpawn->value())
                //    world.addVirus(rand() % world.xsize, rand() % world.ysize, 800);

                world.update();
                tick++;
                ticksNow++;

                nextUpdate = QDateTime::currentMSecsSinceEpoch() + 1000 / targetFps;
            }
            if ((int)QDateTime::currentMSecsSinceEpoch() > lastFpsUpdate + 400) {
                lastFpsUpdate = QDateTime::currentMSecsSinceEpoch();

                if (!is_fast)
                    ui->fps->setText("fps: " + QString::number(targetFps));
                else
                    ui->fps->setText("fps: " + QString::number(ticksNow * 60));
                /*ui->statistics->setText(
                    "cells: " + QString::number(world.physics.countCells()) + "\n" +
                    "joints: " + QString::number(world.physics.countJoints()) + "\n"); //+
                    //"creatures: " + QString::number(world.countCreatures()));*/
            }
            ui->txt_ticks->setText(QString::number(tick));
        }
        this->update();
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
        if (event->key() == ' ') {
            world.update();
            tick++;
        }
        world.keyPress(event->key());
    }

    void wheelEvent(QWheelEvent *event) {
        world.wheel(event->delta() / 120.0);

        event->accept();
    }
};

#endif // MAINWINDOW_H
