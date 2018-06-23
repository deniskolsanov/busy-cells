
#include "entities/creature.h"

int Creature::cellTypeFromGen(double a) {
    return (a + 1) * cellTypes / 2;
}

void Creature::setCellTypeFromState(Cell *cell) {
    int genIndex = gen.size() - 1 + (-genCell + cell->state) * 2;
    cell->type = (cellType)cellTypeFromGen(gen[genIndex]);
}

double Creature::getCellAngleFromState(Cell *cell) {
    int genIndex = gen.size() - 1 + (-genCell + cell->state) * 2;
    return abs(gen[genIndex + 1]) * M_PI * 2;
}

void Creature::update() {
    if (!isAlive())
        return;

    vector<double> input = generateInput();
    vector<double> output = runGen(input);

    for(int i=0; i < memory.size(); i++)
        memory[i] = output[i];

    for(int i=0; i < innerCells.size(); i++) {
        Cell *cell = innerCells[i];

        if (birthKD == 0)
            doEat(cell);
        else
            birthKD--;

        // reduce mass
        cell->mass *= 1 - 0.001 / innerCells.size();

        // die
        if(cell->mass < 100) {
            cell->kill();
            innerCells.erase(innerCells.begin() + i);
            return;
        }

        // drive the engine
        if (cell->type == cellType::ENGINE) {
            double turn = output[output.size() - 2],
                   accel = output[output.size() - 1] + 1;
            cell->addAngle(turn * 0.2);
            cell->applyImpulse(Vect(cell->angle) * accel * 2 / sqrt(cell->getRadius()));
            cell->mass -= accel * 0.1;
        }

        // add new cell in the creature
        if (cell->mass > 300 &&
            cell->state != -1 &&
            cell->state < genCell - 1) {
            double angle = cell->normAngle(cell->angle + getCellAngleFromState(cell));

            cell->mass /= 2;
            Vect pos = cell->pos + Vect(angle);
            auto newCell = cell->world->addCell(pos.x, pos.y, cell->mass);
            newCell->angle = angle;
            newCell->state = cell->state + 1;
            setCellTypeFromState(newCell);
            innerCells.push_back(newCell);
            auto newJoint = cell->world->addJoint(cell, newCell, 0, 0.01);

            cell->state = -1;
            return;
        }

        // spawn new creature
        if(cell->mass > 800 && birthKD == 0) {
            birthKD = 60;
            cell->mass /= 2;
            Vect pos = cell->pos - Vect(cell->angle) * (cell->getRadius() * 2 + 8);
            auto newCreature = cell->world->addCreature(pos.x, pos.y, cell->mass + 300);
            newCreature->innerCells[0]->angle = cell->angle + M_PI;
            newCreature->regenerateFrom(*this);
            return;
        }
    }
}

void Creature::doEat(Cell *cell) {
    for(auto &cell2 : cell->world->cells) {
        if (!cell2.isAlive())
            continue;
        double dist = cell->pos.distTo(cell2.pos) - cell->getRadius() - cell2.getRadius();
        if (*cell != cell2 &&
            dist < 0 &&
            cell->mass > cell2.mass) {
            bool ok = true;
            for(auto &myCell : innerCells)
                if(cell2 == *myCell)
                    ok = false;
            if (!ok)
                continue;

            if (!cell2.isFood() && cell2.mass > 100) {
                double dmass = min(40.0, cell2.mass - 100);
                cell->mass += dmass;
                cell2.mass -= dmass;
            } else if(cell2.isFood()) {
                cell->mass += cell2.mass;
                cell2.kill();
            }
        }
    }
}

vector<double> Creature::runGen(vector<double> &input) {
    int genId = 0;
    for(int lvl=1; lvl < arch.size(); lvl++) {
        vector<double> output(arch[lvl]);
        for(int j=0; j < arch[lvl]; j++) {
            for(int i=0; i < arch[lvl-1]; i++) {
                output[j] += input[i] * gen[genId] * 20;
                genId++;
            }
            output[j] += gen[genId] * 20;
            genId++;
            output[j] = activation(output[j]);
        }

        input = output;
    }
    return input;
}

vector<double> Creature::generateInput() {
    vector<double> input;
    input.reserve(arch[0]);

    for(auto i : memory)
        input.push_back(i);

    for(auto &cell : innerCells) {
        if (cell->type == cellType::EYE) {
            auto eyeInput = cell->generateInput();
            for(int i=0; i < eyeInput.size(); i++)
                input[i] = eyeInput[i];
        }
    }

    return input;
}
