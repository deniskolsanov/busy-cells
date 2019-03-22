
#include "entities/joint.h"

Joint::Joint(Cell *cell1, Cell *cell2, double size, double force) : cell1(cell1), cell2(cell2), size(size), force(force) {
    angle1 = cell1->angleTo(*cell2);
    angle2 = cell2->angleTo(*cell1);
}

bool Joint::isAlive() {
    return cell1 && cell2 && cell1->isAlive() && cell2->isAlive();
}

void Joint::kill() {
    cell1 = nullptr;
    cell2 = nullptr;
}

void Joint::update() {
    if (!isAlive())
        return;

    double averageMass = (cell1->getMass() + cell2->getMass()) / 2;
    cell1->addMass((averageMass - cell1->getMass()) / 100);
    cell2->addMass((averageMass - cell2->getMass()) / 100);

    Vect diff = *cell1 - *cell2;
    double k = (size - diff.size()) * force;

    *cell1 += diff * k;
    cell1->speed += diff * k;

    *cell2 -= diff * k;
    cell2->speed -= diff * k;

    double kk = 1;
    double dangle1 = normAngle( cell1->angleTo(*cell2) - angle1 );
    cell1->addAngle(dangle1 * 0.5 * kk);
    cell2->rotateWithCenter(*cell1, -dangle1 * 0.1 * kk);

    double dangle2 = normAngle( cell2->angleTo(*cell1) - angle2 );
    cell2->addAngle(dangle2 * 0.5 * kk);
    cell1->rotateWithCenter(*cell2, -dangle2 * 0.1 * kk);
}
