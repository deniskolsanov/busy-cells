
#include "entities/joint.h"

Joint::Joint(Cell *cell1, Cell *cell2, double size, double force) : cell1(cell1), cell2(cell2), size(size), force(force) {
    angle1 = cell1->angleTo(*cell2);
    angle2 = cell2->angleTo(*cell1);
}


void Joint::update() {
    if (!isAlive())
        return;

    size = cell1->getRadius() + cell2->getRadius();

    double averageMass = (cell1->mass + cell2->mass) / 2;
    cell1->mass += (averageMass - cell1->mass) / 100;
    cell2->mass += (averageMass - cell2->mass) / 100;

    Vect diff = cell1->pos - cell2->pos;
    double k = (size - diff.size()) * force;

    cell1->pos += diff * k;
    cell1->speed += diff * k;

    cell2->pos -= diff * k;
    cell2->speed -= diff * k;

    double dangle1 = cell1->normAngle( cell1->angleTo(*cell2) - angle1 );
    cell1->addAngle(dangle1 * 0.5);
    cell2->pos.rotateWithCenter(cell1->pos, -dangle1 * 0.1);

    double dangle2 = cell1->normAngle( cell2->angleTo(*cell1) - angle2 );
    cell2->addAngle(dangle2 * 0.5);
    cell1->pos.rotateWithCenter(cell2->pos, -dangle2 * 0.1);
}
