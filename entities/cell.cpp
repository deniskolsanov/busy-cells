
#include "entities/cell.h"

vector<double> Cell::generateInput() {
    vector<double> input(4, 200);
    for(auto &cell : world->cells) {
        double dist = cell.pos.distTo(pos) - getRadius() - cell.getRadius();
        if (cell == *this || dist > 200)
            continue;

        double ang = normAngle(pos.angleTo(cell.pos) - angle);
        if(abs(ang) > M_PI / 2)
            continue;

        // [-pi/2, pi/2] -> [0, pi] -> [0, 4]
        int inputId = (ang + M_PI / 2) * 4 / M_PI;
        if (dist < abs(input[inputId])) {
            if (getRadius() > cell.getRadius())
                input[inputId] = dist;
            else
                input[inputId] = -dist;
        }
    }
    for(int i=0; i < input.size(); i++)
        input[i] = input[i] / 200;
    return input;
}

void Cell::update() {
    if (!isAlive())
        return;

    if (pos.x < 0) {
        speed.x = -speed.x * 0.9;
        pos.x = 0;
    }
    if (pos.x > world->xsize) {
        speed.x = -speed.x * 0.9;
        pos.x = world->xsize;
    }
    if (pos.y < 0) {
        speed.y = -speed.y * 0.9;
        pos.y = 0;
    }
    if (pos.y > world->ysize) {
        speed.y = -speed.y * 0.9;
        pos.y = world->ysize;
    }

    speed *= 0.92;
    pos += speed;
}

void Cell::kill() {
    pos.x = -1000;
    for(auto &joint : world->joints) {
        if (joint.isAlive() && (*joint.cell1 == *this || *joint.cell2 == *this))
            joint.kill();
    }
}
