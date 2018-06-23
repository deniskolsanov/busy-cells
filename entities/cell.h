#ifndef CELL_H
#define CELL_H

#include "vect.h"
#include "world.h"

class World;

const int cellTypes = 3;
enum cellType {
    DEFAULT = 0,
    EYE = 1,
    ENGINE = 2
};

class Cell {
public:
    Vect pos, speed;
    double mass;
    double angle = 0;
    QColor color = Qt::red;

    World *world;

    cellType type = DEFAULT;
    int state = 0;

    Cell(World *world, double x, double y, double mass) :
        world(world),
        pos(Vect(x, y)),
        mass(mass),
        speed(Vect(0, 0)) {}

    bool isAlive() {
        return pos.x != -1000;
    }

    void kill();

    double getRadius() {
        return sqrt(mass / M_PI);
    }

    bool isFood() {
        return color == Qt::green;
    }

    void applyImpulse(const Vect &impulse) {
        pos += impulse;
        speed += impulse;
    }

    void addAngle(const double &dAngle) {
        angle = normAngle(angle + dAngle);
    }

    vector<double> generateInput();
    void update();
    void draw(QPainter &painter);

    bool operator== (const Cell &b) {
        return pos == b.pos;
    }
    bool operator!= (const Cell &b) {
        return pos != b.pos;
    }

    double normAngle(double angle) {
        if(angle < -M_PI)
            return angle + M_PI * 2;
        else if(angle > M_PI)
            return angle - M_PI * 2;
        else
            return angle;
    }

    double angleTo(Cell &cell) {
        return normAngle(pos.angleTo(cell.pos) - angle);
    }
};


#endif // CELL_H
