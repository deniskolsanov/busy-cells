
#include "entities/cell.h"
#include "physics.h"

Cell::Cell(Physics *physics, double x, double y, double radius) :
    Vect(Vect(x, y)),
    physics(physics),
    radius(radius),
    speed(Vect(0, 0)) {}

void Cell::collideWalls() {
    if (x < 0) {
        speed.x = -speed.x * wallDamping;
        x = 0;
    }
    if (x > physics->xsize) {
        speed.x = -speed.x * wallDamping;
        x = physics->xsize;
    }
    if (y < 0) {
        speed.y = -speed.y * wallDamping;
        y = 0;
    }
    if (y > physics->ysize) {
        speed.y = -speed.y * wallDamping;
        y = physics->ysize;
    }
}

void Cell::update() {
    if (!isAlive())
        return;

    if (physics->xsize != 0 && physics->ysize != 0)
        collideWalls();

    speed *= damping;
    *this += speed;
}

bool Cell::isAlive() {
    return x > -1e8;
}

void Cell::kill() {
    x = -1e9;
    /*for(auto &joint : world->joints) {
        if (joint.isAlive() && (*joint.cell1 == *this || *joint.cell2 == *this))
            joint.kill();
    }*/
}

double Cell::getMass() const {
    return square(radius) * M_PI;
}

double Cell::toRadius(const double mass) {
    return sqrt(mass / M_PI);
}

void Cell::addMass(double mass) {
    radius = sqrt(square(radius) + mass / M_PI);
}

void Cell::applyImpulse(const Vect impulse) {
    *this += impulse;
    speed += impulse;
}

void Cell::addAngle(const double dAngle) {
    angle = normAngle(angle + dAngle);
}

bool Cell::operator== (const Cell &b) {
    return *this == b;
}
bool Cell::operator!= (const Cell &b) {
    return *this != b;
}

double Cell::angleTo(Cell &cell) {
    return normAngle(angleTo(cell) - angle);
}

int Cell::hash() {
    return this->hash();
}
