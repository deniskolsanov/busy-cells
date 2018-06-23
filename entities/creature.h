#ifndef CREATURE_H
#define CREATURE_H

#include "world.h"

class Cell;

class Creature {
public:
    int genCell = 2;
    vector<int> arch = {6, 10, 10, 10, 6};
    vector<double> gen;
    vector<double> memory;

    vector<Cell*> innerCells;

    int birthKD = 0;

    Creature(Cell *cell) {
        int genSize = genCell * 2 + 1;
        for(int i=1; i < arch.size(); i++)
            genSize += arch[i-1] * arch[i] + arch[i];
        for(int i=0; i < genSize; i++)
            gen.push_back(rand() * 2.0 / RAND_MAX - 1);

        innerCells.push_back(cell);
        setCellTypeFromState(cell);

        memory = vector<double> (arch[0]);
    }

    bool isAlive() {
        return innerCells.size();
    }

    void regenerateFrom(Creature &creature) {
        double step = abs(creature.gen.back()) * 0.2;
        //step = 0.1;
        for(int i=0; i < creature.gen.size(); i++)
            gen[i] = min(max(creature.gen[i] + rand() * step / RAND_MAX - step / 2, -1.0), 1.0);
        setCellTypeFromState(innerCells.front());
    }

    int cellTypeFromGen(double a);
    void setCellTypeFromState(Cell *cell);
    double getCellAngleFromState(Cell *cell);

    double activation(double x) {
        return x / sqrt(1 + x * x);
    }

    vector<double> runGen(vector<double> &input);
    vector<double> generateInput();
    void update();
    void doEat(Cell *cell);
};

#endif // CREATURE_H
