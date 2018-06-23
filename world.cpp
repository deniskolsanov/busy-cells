
#include "world.h"

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

Cell* World::addCell(double x, double y, double mass) {
    for(int i=0; i < cells.size(); i++) {
        if(!cells[i].isAlive()) {
            cells[i] = Cell(this, x, y, mass);
            return &cells[i];
        }
    }
    cells.push_back(Cell(this, x, y, mass));
    return &cells.back();
}
Joint* World::addJoint(Cell *cell1, Cell *cell2, double size, double force) {
    for(int i=0; i < joints.size(); i++) {
        if(!joints[i].isAlive()) {
            joints[i] = Joint(cell1, cell2, size, force);
            return &joints[i];
        }
    }
    joints.push_back(Joint(cell1, cell2, size, force));
    return &joints.back();
}
Creature* World::addCreature(double x, double y, double mass) {
    Cell *cell = addCell(x, y, mass);
    for(int i=0; i < creatures.size(); i++) {
        if(!creatures[i].isAlive()) {
            creatures[i] = Creature(cell);
            return &creatures[i];
        }
    }
    creatures.push_back(Creature(cell));
    return &creatures.back();
}
Virus* World::addVirus(double x, double y, double mass) {
    Cell *cell = addCell(x, y, mass);
    for(int i=0; i < viruses.size(); i++) {
        if(!viruses[i].isAlive()) {
            viruses[i] = Virus(cell);
            return &viruses[i];
        }
    }
    viruses.push_back(Virus(cell));
    return &viruses.back();
}

void World::collideRows(vector<Cell*> &up, vector<Cell*> &down) {
    int downI = 0;
    for(int i=0; i < up.size(); i++) {
        auto &cell = up[i];
        for(; downI < down.size(); downI++)
            if (cell->pos.x - down[downI]->pos.x < max_cell_size * 2)
                break;
        for(int upI=i+1; upI < up.size(); upI++) {
            if (up[upI]->pos.x - cell->pos.x > up[upI]->getRadius() + cell->getRadius())
                break;
            collide(*cell, *up[upI]);
        }
        for(int downJ = downI; downJ < down.size(); downJ++) {
            if (down[downJ]->pos.x - cell->pos.x > down[downJ]->getRadius() + cell->getRadius())
                break;
            collide(*cell, *down[downJ]);
        }
    }
}

void World::update() {
    // update cells
    for(int cell=0; cell < cells.size(); cell++)
        cells[cell].update();
    // update joints
    for(auto &joint : joints)
        joint.update();
    for(auto &virus : viruses)
        virus.update();


    // push cells in rows
    vector< vector<Cell*> > sorted(ysize / (max_cell_size * 1.4) + 2);
    for(auto &cell : cells) {
        if (cell.isAlive()) {
            int index = clamp(int(cell.pos.y / (max_cell_size * 1.4)), 0, (int)sorted.size() - 1);
            sorted[index].push_back(&cell);
        }
    }
    // sort each row of cells by x field
    for(auto &row : sorted)
        sort(row.begin(), row.end(),
             [](const Cell *cell1, const Cell *cell2) -> bool {return cell1->pos.x < cell2->pos.x;});
    // collide cells on rows and between rows
    for(int rowI=0; rowI < sorted.size()-1; rowI++) {
        auto &up = sorted[rowI],
             &down = sorted[rowI + 1];
        collideRows(up, down);
    }

    // update creatures
    for(int creature=0; creature < creatures.size(); creature++)
        creatures[creature].update();

    // old, but pretty simple O(N^2) collision detection algorithm
    /*for(int cell1=0; cell1 < cells.size(); cell1++) {
        if(!cells[cell1].isAlive())
            continue;
        for(int cell2=cell1+1; cell2 < cells.size(); cell2++) {
            if(!cells[cell2].isAlive())
                continue;
            collide(cells[cell1], cells[cell2]);
        }
    }*/
}

void World::collide(Cell &cell1, Cell &cell2) {
    double dist = cell1.pos.distTo(cell2.pos);
    if(cell1 == cell2 || dist > cell1.getRadius() + cell2.getRadius())
        return;
    Vect diff = cell1.pos - cell2.pos;
    double k = 1 - sqrt(dist / (cell1.getRadius() + cell2.getRadius()));

    cell1.applyImpulse(diff * k * cell2.mass * 2 / (cell1.mass + cell2.mass));
    cell2.applyImpulse(diff * (-1) * k * cell1.mass * 2 / (cell1.mass + cell2.mass));
}

void World::draw(QPainter &painter) {
    for(auto &cell : cells)
        cell.draw(painter);
    for(auto &joint : joints)
        joint.draw(painter);
}

int World::countCells() {
    int n = 0;
    for(auto &i : cells)
        if (i.isAlive())
            n++;
    return n;
}

int World::countJoints() {
    int n = 0;
    for(auto &i : joints)
        if (i.isAlive())
            n++;
    return n;
}

int World::countCreatures() {
    int n = 0;
    for(auto &i : creatures)
        if (i.isAlive())
            n++;
    return n;
}
