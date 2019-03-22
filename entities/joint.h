#ifndef JOINT_H
#define JOINT_H

#include <QPainter>
#include "vect.h"
#include "cell.h"

class Cell;

class Joint {
public:
    Cell *cell1 = nullptr, *cell2 = nullptr;
    double size, force;

    double angle1, angle2;
    Joint(Cell *cell1, Cell *cell2, double size, double force);

    bool isAlive();
    void kill();

    void update();
};

#endif // JOINT_H
