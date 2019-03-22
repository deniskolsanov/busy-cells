
#include "entities/creature.h"

Creature::Creature(World *world, Cell *cell) : world(world) {
    int genSize = calculateArch(arch) + calculateArch(arch2);
    for(int i=0; i < genSize; i++)
        gen.push_back(rand() * 2.0 / RAND_MAX - 1);

    innerCells.push_back(cell);

    memory = vector<double> (arch[0]);
}

bool Creature::isAlive() {
    return innerCells.size();
}

double Creature::normMass(double mass) { // get mass for NN [-1, 1]
    return min(0.08 * sqrt(max(mass, 100.0)-100) - 1, 1.0);
}

int Creature::cellTypeFromGen(double a) {
    return (a + 1) * cellTypes / 2;
}

double Creature::cellAngleFromGen(double a) {
    return a * M_PI;
}
int Creature::cellMarkFromGen(double a) {
    return (a + 1) * 2;
}

void Creature::initFirstCell(Creature &creature) {
    vector<double> input = {0, 0, 0, 0, 0};
    vector<double> output = runNN(1, input);

    Cell *cell = creature.innerCells[0];
    cell->type = (cellType)output[0];
    cell->mark = cellMarkFromGen(output[1]);
    cell->angle = cell->normAngle(cell->angle + cellAngleFromGen(output[2]));
    if (cell->type == NIL) {
        cell->kill();
        creature.innerCells.erase(innerCells.begin());
    }
}

void Creature::regenerateFrom(Creature &creature) {
    double step = abs(creature.gen.back()) * 0.2;
    step = 0.1;
    for(int i=0; i < creature.gen.size(); i++)
        gen[i] = min(max(creature.gen[i] + rand() * step / RAND_MAX - step / 2, -1.0), 1.0);

    initFirstCell(creature);
}

void Creature::update() {
    if (!isAlive())
        return;

    vector<double> input = generateInput();
    vector<double> output = runNN(0, input);

    for(int i=0; i < memory.size(); i++)
        memory[i] = output[i];

    for(int i=0; i < innerCells.size(); i++) {
        Cell *cell = innerCells[i];

        doEat(cell);
        if (birthKD > 0)
            birthKD--;

        // reduce mass
        cell->mass *= 1 - 0.0005 * innerCells.size();
        cell->mass = min(cell->mass, 800.0);

        // drive the engine
        if (cell->type == cellType::ENGINE) {
            double turn = output[output.size() - 2],
                   accel = output[output.size() - 1] + 1;
            cell->addAngle(turn * 0.2);
            cell->applyImpulse(Vect(cell->angle) * accel * 2 / sqrt(cell->getRadius()));
            cell->mass -= accel * 0.1;
        }

        if (checkCellDie(cell))
            return;
        if (checkCellDivision(cell))
            return;
    }
}

bool Creature::checkCellDie(Cell *cell) {
    if(cell->mass < 100) {
        cell->kill();
        innerCells.erase(find(innerCells.begin(), innerCells.end(), cell));
        for(int i=0; i < innerJoints.size();) {
            auto &joint = innerJoints[i];
            if (*joint->cell1 == *cell || *joint->cell2 == *cell) {
                joint->kill();
                innerJoints.erase(innerJoints.begin() + i);
            } else
                i++;
        }
        return true;
    }
    return false;
}

bool Creature::checkCellDivision(Cell *cell) {
    if (cell->mass > 200 && !birthKD && rand() % 60 == 0) {

        vector<double> input = calculateMarks(cell); // inputs: 4 from cell marks, cell mass
        input.push_back(normMass(cell->mass));
        vector<double> output = runNN(1, input); // outputs: cell type, cell mark, relative cell angle
        cellType type = (cellType)cellTypeFromGen(output[0]);
        fflush(stdout);
        int mark = cellMarkFromGen(output[1]);
        if (type == NIL)
            return false;

        double angle = cell->normAngle(cell->angle + cellAngleFromGen(output[2]));
        cell->mass /= 2;
        Vect pos = cell->pos + Vect(angle);
        if (cell->mark != mark) { // append to existing Creature
            birthKD = 20;
            cell->mass += 50;
            auto newCell = cell->physics->addCell(pos.x, pos.y, cell->mass);
            newCell->angle = angle;
            newCell->type = type;
            innerCells.push_back(newCell);
            auto newJoint = cell->physics->addJoint(cell, newCell, 0, 0.01 * 0.1);
            innerJoints.push_back(newJoint);
        } else { // create new Creature
            birthKD = 60;
            auto newCreature = world->addCreature(pos.x, pos.y, cell->mass + 50);
            newCreature->innerCells[0]->angle = cell->angle;
            newCreature->regenerateFrom(*this);
        }

        return true;
    }
    return false;
}

void Creature::doEat(Cell *cell) {
    for(auto &cell2 : cell->physics->cells) {
        if (!cell2.isAlive())
            continue;
        double dist = cell->pos.distTo(cell2.pos) - cell->getRadius() - cell2.getRadius();


        if (*cell != cell2 &&
            dist < 0 &&
            cell->mass > cell2.mass) {
            bool isMy = false;
            for(auto &myCell : innerCells)
                if(cell2 == *myCell)
                    isMy = true;
            if (isMy) {
                // decrease our mass
                //double dmass = min(4.0, cell->mass - 100);
                //cell->mass -= dmass;
            } else {
                // eat enemy
                if (!cell2.type == FOOD && cell2.mass > 100 && !birthKD) {
                    double dmass = min(40.0, cell2.mass - 100);
                    cell->mass += dmass;
                    cell2.mass -= dmass;
                } else if(cell2.type == FOOD) {
                    cell->mass += cell2.mass * 2; // double mass from food
                    cell2.kill();
                }
            }
        }
    }
}

double Creature::activation(double x) {
    return x / sqrt(1 + x * x);
}

int Creature::calculateArch(vector<int> &arch) {
    int n = 1;
    for(int i=1; i < arch.size(); i++)
        n += arch[i-1] * arch[i] + arch[i];
    return n;
}

vector<double> Creature::runNN(int archN, vector<double> &input) {
    int genId = 0;
    vector<int> &curArch = arch;
    if (archN != 0) {
        genId = calculateArch(arch);
        curArch = arch2;
    }
    for(int lvl=1; lvl < curArch.size(); lvl++) {
        vector<double> output(curArch[lvl]);
        for(int j=0; j < curArch[lvl]; j++) {
            for(int i=0; i < curArch[lvl-1]; i++) {
                output[j] += input[i] * gen[genId] * 2;
                genId++;
            }
            output[j] += gen[genId] * 2;
            genId++;
            output[j] = activation(output[j]);
        }

        input = output;
    }
    return input;
}

vector<double> Creature::generateInputFromCell(Cell &me) {
    vector<double> input(4, 200);
    for(auto &cell : world->physics.cells) {
        double dist = cell.pos.distTo(me.pos) - me.getRadius() - cell.getRadius();
        if (cell == me || dist > 200)
            continue;

        double ang = me.normAngle(me.pos.angleTo(cell.pos) - me.angle);
        if(abs(ang) > M_PI / 2)
            continue;

        // [-pi/2, pi/2] -> [0, pi] -> [0, 4]
        int inputId = (ang + M_PI / 2) * 4 / M_PI;
        if (dist < abs(input[inputId])) {
            if (me.getRadius() > cell.getRadius())
                input[inputId] = dist;
            else
                input[inputId] = -dist;
        }
    }
    for(int i=0; i < input.size(); i++)
        input[i] = input[i] / 200;
    return input;
}

vector<double> Creature::generateInput() {
    vector<double> input;
    input.reserve(arch[0]);

    for(auto i : memory)
        input.push_back(i);

    for(auto &cell : innerCells) {
        if (cell->type == cellType::EYE) {
            auto eyeInput = generateInputFromCell(*cell);
            for(int i=0; i < eyeInput.size(); i++)
                input[i] = eyeInput[i];
        }
    }

    return input;
}

vector<double> Creature::calculateMarks(Cell *cell) { // uses BFS algorithm
    vector<double> marks(4, -1);
    marks[cell->mark]++;

    map<int, bool> checked;
    checked[cell->hash()] = true;

    queue< pair<Cell*, double> > q;
    q.push({cell, 0.5});

    while(q.size()) {
        cell = q.front().first;
        double k = q.front().second;
        q.pop();

        for(auto &joint : innerJoints) {
            if (checked[joint->cell1->hash()] &&
                checked[joint->cell2->hash()] ||
                *joint->cell1 != *cell && *joint->cell2 != *cell)
                continue;
            Cell *cell2 = joint->cell1;
            if (cell2 == cell)
                cell2 = joint->cell2;

            checked[cell2->hash()] = true;
            marks[cell2->mark] += k;
            q.push({cell2, k / 2});
        }
    }

    return marks;
}
