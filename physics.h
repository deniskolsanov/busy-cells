#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <vector>
#include "entities/cell.h"
#include "entities/joint.h"

class Cell;
class Joint;

class Physics {
public:
    static const int max_cell_size = 16;
    int xsize = 0, ysize = 0;
    Vect gravity;

    vector<Cell> cells;
    vector<Joint> joints;

    void (*collideCallback)(Cell&, Cell&);

    Physics(int xsize, int ysize);
    Physics(Physics *Physics);
    Physics& operator=(const Physics &physics);
    void setCollideCallback(void cb(Cell&, Cell&));

    Cell* addCell(double x, double y, double mass);
    Joint* addJoint(Cell *cell1, Cell *cell2, double size, double force);

    vector< vector<Cell*> > sorted;
    void collide(Cell &cell, Cell &cell2);
    void collideRows(vector<Cell*> &up, vector<Cell*> &down);
    void update();

    size_t countCells();
    size_t countJoints();
};

#endif // PHYSICS_H
