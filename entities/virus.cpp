

#include "entities/virus.h"

bool Virus::isAlive() {
    return cell && cell->isAlive();
}

Virus::Virus(Cell *cell) {
    this->cell = cell;
    cell->color = Qt::magenta;
    cell->angle = cell->normAngle(rand());
}

void Virus::update() {
    if (!isAlive())
        return;

    for(auto &cell2 : cell->world->cells) {
        if (!cell2.isAlive())
            continue;
        double dist = cell->pos.distTo(cell2.pos) - cell->getRadius() - cell2.getRadius();
        if (*cell != cell2 &&
            dist < 0 &&
            cell->mass > cell2.mass &&
            cell2.color != Qt::magenta) {

            if (!cell2.isFood() && cell2.mass > 100) {
                double dmass = min(40.0, cell2.mass - 100);
                cell->mass += dmass / 2;
                cell2.mass -= dmass;
            } else if (cell2.isFood()) {
                cell->mass += cell2.mass / 2;
                cell2.kill();
            }
        }
    }

    cell->mass *= 0.9995;
    if(cell->mass < 600) {
        cell->kill();
        cell = nullptr;
        return;
    }

    if(cell->mass > 800) {
        cell->mass = 700;
        Vect pos = cell->pos - Vect(cell->angle) * 1;
        auto newVirus = cell->world->addVirus(pos.x, pos.y, cell->mass);
        newVirus->cell->angle = cell->angle + M_PI;
        return;
    }
}
