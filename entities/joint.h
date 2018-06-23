#ifndef JOINT_H
#define JOINT_H

#include "vect.h"
#include "cell.h"

class Cell;

class Joint {
public:
    Cell *cell1 = nullptr, *cell2 = nullptr;
    double size, force;

    double angle1, angle2;
    Joint(Cell *cell1, Cell *cell2, double size, double force);

    bool isAlive() {
        return cell1 && cell2;
    }

    void kill() {
        cell1 = nullptr;
        cell2 = nullptr;
    }

    void update();

    void draw(QPainter &painter);
};

#endif // JOINT_H
