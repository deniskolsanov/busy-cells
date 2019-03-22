#include "physics.h"

/*template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}*/

Physics::Physics(int xsize, int ysize) : xsize(xsize), ysize(ysize) {
    // this is to prevent pointer scraping after vector resizing
    cells.reserve(10000);
    joints.reserve(10000);
}

Physics::Physics(Physics *physics)
    : xsize(physics->xsize),
      ysize(physics->ysize),
      cells(physics->cells),
      joints(physics->joints),
      gravity(physics->gravity),
      collideCallback(physics->collideCallback) {

    cells.reserve(10000);
    joints.reserve(10000);

    for (auto &cell : cells)
        cell.physics = this;
}

Physics& Physics::operator=(const Physics &physics) {
    xsize = physics.xsize;
    ysize = physics.ysize;
    cells = physics.cells;
    joints = physics.joints;
    gravity = physics.gravity;
    collideCallback = physics.collideCallback;
    for (auto &cell : cells)
        cell.physics = this;

    return *this;
}

void Physics::setCollideCallback(void cb(Cell&, Cell&)) {
    collideCallback = cb;
}

Cell* Physics::addCell(double x, double y, double radius) {
    for(size_t i=0; i < cells.size(); i++) {
        if(!cells[i].isAlive()) {
            cells[i] = Cell(this, x, y, radius);
            return &cells[i];
        }
    }
    cells.push_back(Cell(this, x, y, radius));
    return &cells.back();
}

Joint* Physics::addJoint(Cell *cell1, Cell *cell2, double size, double force) {
    for(size_t i=0; i < joints.size(); i++) {
        if(!joints[i].isAlive()) {
            joints[i] = Joint(cell1, cell2, size, force);
            return &joints[i];
        }
    }
    joints.push_back(Joint(cell1, cell2, size, force));
    return &joints.back();
}

void Physics::collide(Cell &cell, Cell &cell2) {
    double dist = cell.radius + cell2.radius;
    if(cell.dist2To(cell2) <= dist * dist)
        (*collideCallback)(cell, cell2);
}

void Physics::collideRows(vector<Cell*> &up, vector<Cell*> &down) {
    size_t downI = 0;
    for(size_t i=0; i < up.size(); i++) {
        auto &cell = up[i];
        for(; downI < down.size(); downI++)
            if (cell->x - down[downI]->x < max_cell_size * 2)
                break;
        for(size_t upI=i+1; upI < up.size(); upI++) {
            auto cell2 = up[upI];
            if (cell2->x - cell->x > cell2->radius + cell->radius)
                break;

            collide(*cell, *cell2);
        }
        for(size_t downJ = downI; downJ < down.size(); downJ++) {
            auto cell2 = down[downJ];
            if (cell2->x - cell->x > cell2->radius + cell->radius)
                break;

            collide(*cell, *cell2);
        }
    }
}

void Physics::update() {
    // update cells
    for(auto &cell : cells) {
        cell.applyImpulse(gravity);
        cell.update();
    }
    // update joints
    for(auto &joint : joints)
        joint.update();

    // push cells in rows
    sorted.resize(ysize / (max_cell_size * 1.4) + 2);
    for (auto &i : sorted)
        i.resize(0);

    for(auto &cell : cells) {
        if (cell.isAlive()) {
            size_t index = clamp<size_t>(cell.y / (max_cell_size * 1.4), 0, sorted.size() - 1);
            sorted[index].push_back(&cell);
            //sorted[0].push_back(&cell);
        }
    }
    // sort each row of cells by x field
    for(auto &row : sorted) {
        sort(row.begin(), row.end(),
             [](const Cell *cell1, const Cell *cell2) -> bool { return cell1->x < cell2->x; });
    }
    // collide cells on rows and between rows
    for(size_t rowI=0; rowI < sorted.size()-1 && sorted.size(); rowI++) {
        auto &up = sorted[rowI],
             &down = sorted[rowI + 1];
        collideRows(up, down);
    }
}

size_t Physics::countCells() {
    size_t n = 0;
    for(auto &i : cells)
        if (i.isAlive())
            n++;
    return n;
}

size_t Physics::countJoints() {
    size_t n = 0;
    for(auto &i : joints)
        if (i.isAlive())
            n++;
    return n;
}
