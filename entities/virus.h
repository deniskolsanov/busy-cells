#ifndef VIRUS_H
#define VIRUS_H

#include "world.h"
//#include "entities/cell.h"

//class Cell;
//class World;

class Virus {
public:
    Cell *cell = nullptr;
    Virus(Cell *cell);

    bool isAlive();
    void update();
};

#endif // VIRUS_H
