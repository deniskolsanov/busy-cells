#ifndef CREATURE_H
#define CREATURE_H

#include "world.h"
#include <queue>
#include <map>

class Cell;
class Joint;

class Creature {
public:
    // -- Neuro Net architectures:
    // brain
    // inputs: 4 from eye, 2 from memory
    // outputs: 2 for movement, 2 for memory
    vector<int> arch = {6, 10, 10, 10, 6};

    // structure builder
    // inputs: 4 from cell marks, cell mass
    // outputs: cell type, cell mark, relative cell angle
    vector<int> arch2 = {5, 8, 8, 3};

    World *world;

    vector<double> gen;
    vector<double> memory;

    vector<Cell*> innerCells;
    vector<Joint*> innerJoints;

    int birthKD = 60;

    Creature(World *world, Cell *cell);
    bool isAlive();
    double normMass(double mass);
    void initFirstCell(Creature &creature);
    void regenerateFrom(Creature &creature);

    int cellTypeFromGen(double a);
    double cellAngleFromGen(double a);
    int cellMarkFromGen(double a);
    double getCellAngleFromState(Cell *cell);

    double activation(double x);

    int calculateArch(vector<int> &arch);
    vector<double> runNN(int archN, vector<double> &input);
    vector<double> generateInputFromCell(Cell &me);
    vector<double> generateInput();
    void update();
    bool checkCellDie(Cell *cell);
    bool checkCellDivision(Cell *cell);
    void doEat(Cell *cell);

    vector<double> calculateMarks(Cell *cell);
};

#endif // CREATURE_H
