#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <QPainter>
#include <QPoint>
#include "vect.h"

#include "physics.h"
#include "nn.h"

class World {
public:
    static const int max_cell_size = 16; // maximum stable cell radius for physics engine
    double scale = 1;
    Vect mousePosition, clickPos;
    Cell *selectedCell = nullptr;

    Vect cameraPosition;
    int xsize = 800,
        ysize = 1000;

    Physics physics = Physics(xsize, ysize);

    vector<pair<float, NN>> population;

    World();

    void prepareGraph(Physics &physics);
    vector<float> calculateMark(Cell &cell);
    void update();
    void draw(QPainter &painter);

    void growPlant(Physics &physics, NN &nn);
    void optimizeJoints(Physics &physics);

    void mousePress(int x, int y);
    void mouseMove(int x, int y);
    void mouseRelease(int x, int y);

    void keyPress(int key);
    void wheel(double delta);

    void draw(Cell &cell, QPainter &painter);
    void draw(Joint &joint, QPainter &painter);
};

#endif // WORLD_H
