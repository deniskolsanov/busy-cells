#ifndef CELL_H
#define CELL_H

#include <QPainter>
#include "vect.h"

class Physics;
class World;

const int cellTypes = 4;
enum cellType {
    NIL = 0,
    DEFAULT = 1,
    EYE = 2,
    ENGINE = 3,
    FOOD = 4,
    SOIL = 5,
    PLANT = 6
};

class Cell : public Vect {
public:
    Vect speed;
    double radius;

    double angle = 0;
    double damping = 0.98;
    double wallDamping = 0.8;
    QColor color = Qt::red;

    Physics *physics;

    cellType type = NIL;
    int mark = 0;

    Cell(Physics *physics, double x, double y, double mass);

    bool isAlive();
    void kill();
    double getMass() const;
    static double toRadius(const double mass);
    void addMass(const double mass);
    void applyImpulse(const Vect impulse);
    void addAngle(const double dAngle);

    void collideWalls();
    void update();

    bool operator== (const Cell &b);
    bool operator!= (const Cell &b);

    double angleTo(Cell &cell);
    int hash();
};


#endif // CELL_H
