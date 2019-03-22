#ifndef WORLD2_H
#define WORLD2_H

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <QPainter>
#include <QPoint>
#include "vect.h"

#include "world.h"
#include "nn.h"
#include "quads.h"
#include "hsvtorgb.h"
#include <fstream>

enum Types {
    EMPTY = 0,
    WALL,
    BOT,
    MEAT
};

class World2 : public World {
public:
    double scale = 1;
    Vect mousePosition, clickPos;

    Vect cameraPosition;
    size_t xsize = 100,
           ysize = 80;

    Quads<Types> quads = Quads<Types>(xsize, ysize);


    vector<pair<float, NN>> population;

    World2();

    void updateQuad(const size_t x, const size_t y);
    void update();
    void draw(QPainter &painter);

    void mousePress(int x, int y);
    void mouseMove(int x, int y);
    void mouseRelease(int x, int y);

    void keyPress(int key);
    void wheel(double delta);
};

#endif // WORLD2_H
