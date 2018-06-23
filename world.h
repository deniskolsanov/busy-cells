#ifndef WORLD_H
#define WORLD_H

#include <QPainter>
#include <QPoint>
#include "vect.h"
#include "entities/cell.h"
#include "entities/joint.h"
#include "entities/creature.h"
#include "entities/virus.h"

class Virus;
class Joint;
class Creature;

class World {
public:
    static const int max_cell_size = 16; // maximum stable cell radius for physics engine
    double scale = 1;
    Vect mousePosition;

    // world entities
    vector<Cell> cells;
    vector<Joint> joints;
    vector<Creature> creatures;
    vector<Virus> viruses;

    Vect cameraPosition;
    int xsize = 2000,
        ysize = 2000;

    Cell* addCell(double x, double y, double mass);
    Joint* addJoint(Cell *cell1, Cell *cell2, double size, double force);
    Creature* addCreature(double x, double y, double mass);
    Virus* addVirus(double x, double y, double mass);

    World() {
        // this is to prevent pointer scraping after vector resizing
        cells.reserve(10000);
        joints.reserve(10000);
        creatures.reserve(10000);
        viruses.reserve(10000);
    }

    void collideRows(vector<Cell*> &up, vector<Cell*> &down);
    void collide(Cell &cell1, Cell &cell2);
    void update();
    void draw(QPainter &painter);

    void mousePress(int x, int y) {
        mousePosition.set(x, y);
    }
    void mouseMove(int x, int y) {
        if (mousePosition != Vect(0, 0)) {
            cameraPosition += (Vect(x, y) - mousePosition) / scale;
            mousePosition.set(x, y);
        }
    }
    void mouseRelease(int x, int y) {
        mousePosition.set(0, 0);
    }
    void keyPress(int key) {}
    void wheel(double delta) {
        double lscale = scale;
        scale *= pow(1.2, delta);
        cameraPosition += Vect(330, 330) * (1 / scale - 1 / lscale);
    }
    int countCells();
    int countJoints();
    int countCreatures();
};

#endif // WORLD_H
