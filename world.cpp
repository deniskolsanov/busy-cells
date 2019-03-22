
#include "world.h"

/*template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}*/

void collide(Cell &cell1, Cell &cell2) {
    Vect diff = cell1 - cell2;
    //double k = 1 - sqrt(diff.size() / (cell1.getRadius() + cell2.getRadius()));

    double k = (1 - pow(diff.size() / (cell1.radius + cell2.radius), 0.25));

    cell1.applyImpulse(diff * k * cell2.getMass() * 2 / (cell1.getMass() + cell2.getMass()));
    cell2.applyImpulse(diff * (-1) * k * cell1.getMass() * 2 / (cell1.getMass() + cell2.getMass()));
}

float calcFitness(Physics &physics) {
    /*float fitness = 1e9;

    for (auto &cell : physics.cells) {
        //fitness += cell.pos.y;
        fitness = min(fitness, (float)cell.pos.y);
    }*/

    float fitness = 15000;
    double maxHeight = 1e9;
    for (auto &cell : physics.cells) {
        if (cell.type != DEFAULT)
            continue;

        if (cell.x > 10 && cell.x < cell.physics->xsize - 10)
            maxHeight = min(maxHeight, cell.y);

        if (cell.x < 10 ||
                cell.x > cell.physics->xsize - 10 ||
                cell.y > cell.physics->ysize - 10)
            fitness += 100;

        bool isUnderDirt = false;
        for (auto &cell2 : physics.cells) {
            if (cell2.type == DEFAULT)
                continue;

            if (cell.distTo(cell2) < cell.radius + cell2.radius + 6) {
                fitness -= cell2.getMass();
                cell2.radius = 0;
            }

            double angle = (cell2 - cell).getAngle();
            if ( angle > -M_PI * 3 / 4 && angle < -M_PI / 4 )
                isUnderDirt = true;
        }
        if (isUnderDirt)
            fitness -= 100;
    }

    fitness += pow(maxHeight, 2) / 2;

    return fitness;
}

World::World() {
    // test neuro net
    /*vector<pair<float, NN>> q;
    for (int i=0; i < 10; i++)
        q.push_back({0, NN({2, 10, 10, 10, 10, 1})});

    auto t = clock();

    float range = 1;
    int lastChange = 0;
    vector<float> g(1000);
    for (int step=0; step < 100; step++) {
        for (auto &i : q) {
            i.first = 0;
            for (float x=-1; x < 1; x += 0.01)
                i.first += pow(i.second.run({x, rand() * 1.0 * 0 / RAND_MAX})[0] - 0.25 - x * x / 2, 2);
        }
        sort(q.begin(), q.end(), [] (const pair<float, NN> &a, const pair<float, NN> &b) {return a.first < b.first;});
        for (int i=q.size() / 2; i < q.size(); i++) {
            q[i].second.cloneFrom(q[i-q.size() / 2].second)->addRandom(range);
        }

        g[step] = q[0].first;
        if (step - lastChange > 10 && g[step] == g[step-10]) {
            lastChange = step;
            range *= 0.8;
        }
    }
    cout << "error: " << q[0].first << endl;
    cout << "time: " << clock() - t << endl;*/


    physics.gravity = Vect(0, 0.02);
    physics.setCollideCallback(collide);

    for (int i=0; i < 2000; i++) {
        auto newCell = physics.addCell(rand() % xsize, ysize / 2 + rand() % 200, Cell::toRadius(rand() % 200 + 20));
        newCell->color = QColor(185, 120, 90);
    }
    for (int i=0; i < 400; i++)
        physics.update();

    auto cell = physics.addCell(xsize / 2, ysize - 100, Cell::toRadius(100));
    cell->type = DEFAULT;
    cell->color = Qt::green;

    for (int i=0; i < 200; i++)
        physics.update();

    Physics basisPhysics {&physics};

    for (int i=0; i < 12; i++)
        population.push_back({0, NN({5, 10, 20, 10, 7})});

    auto t = clock();

    /*float range = 0.2;
    int lastChange = 0;
    vector<float> g(1000);
    for (int step=0; step < 2; step++) {
        for (auto &i : population) {
            Physics p {&basisPhysics};
            for (int tick=0; tick < 1000; tick++) {
                if (rand() % 20 == 0) {
                    optimizeJoints(p);
                    growPlant(p, i.second);
                }

                p.update();
            }
            i.first = calcFitness(p);
        }
        sort(population.begin(), population.end(), [] (const pair<float, NN> &a, const pair<float, NN> &b) {return a.first < b.first;});
        for (int i=population.size() / 2; i < population.size(); i++) {
            population[i].second.cloneFrom(population[i-population.size() / 2].second)->addRandom(range);
        }

        g[step] = population[0].first;
        if (step - lastChange > 10 && g[step] == g[step-10]) {
            lastChange = step;
            range *= 0.8;
        }

        cout << "error: " << population[0].first << endl;
    }
    cout << "error: " << population[0].first << endl;
    cout << "time: " << clock() - t << endl;*/
}

unordered_map< int, vector<Cell*> > g;
void World::prepareGraph(Physics &physics) {
    g.clear();
    for (auto &joint : physics.joints) {
        g[joint.cell1->hash()].push_back(joint.cell2);
        g[joint.cell2->hash()].push_back(joint.cell1);
    }
}

vector<float> World::calculateMark(Cell &targetCell) {
    vector<float> marks(4);

    marks[targetCell.mark] += 1;

    unordered_set<int> visited;
    visited.insert(targetCell.hash());
    vector<Cell*> q;
    q.push_back(&targetCell);
    for(float m=0.5; q.size(); m /= 2.0) {
        vector<Cell*> w;
        for (auto &cell : q) {
            for (auto &otherCell : g[cell->hash()]) {
                if (visited.count(otherCell->hash()))
                    continue;

                marks[otherCell->mark] += m;
                w.push_back(otherCell);
                visited.insert(otherCell->hash());
            }
        }

        swap(q, w);
    }

    for (int i=0; i < marks.size(); i++)
        marks[i] = marks[i] / 4 - 0.5;

    return marks;
}

void World::update() {
    if (rand() % 20 == 0) {
        //optimizeJoints(physics);
        //growPlant(physics, population[0].second);
    }

    physics.update();

    if (selectedCell && !selectedCell->isAlive())
        selectedCell = nullptr;
}

void World::growPlant(Physics &physics, NN &nn) {
    Vect averagePos;
    int posN = 0;

    prepareGraph(physics);
    Cell *bcell;
    float best = -100;
    int bmark;
    double bmass, bangle;
    for (auto &cell : physics.cells) {
        if (cell.type == DEFAULT) {
            averagePos += cell;
            posN++;

            auto input = calculateMark(cell);
            input.push_back(cell.y / ysize - 0.5);
            auto output = nn.run(input);
            if (output[0] > best) {
                best = output[0];
                bcell = &cell;
                bmass = 40 + (clamp((double)output[5], -0.5, 0.5) + 0.5) * 100;
                bangle = clamp((double)output[6], -0.5, 0.5) * 3;

                if ( output[1] > output[2] &&
                     output[1] > output[3] &&
                     output[1] > output[4] )
                    bmark = 0;
                else if (output[2] > output[3] &&
                         output[2] > output[4])
                    bmark = 1;
                else if (output[3] > output[4])
                    bmark = 2;
                else
                    bmark = 3;
            }
        }
    }

    averagePos /= posN;
    double splitAngle = (*bcell - averagePos).getAngle() + bangle;

    auto cell = bcell;
    auto cell2 = physics.addCell(cell->x + cos(splitAngle) * 4, cell->y + sin(splitAngle) * 4, Cell::toRadius(bmass));
    physics.addJoint(cell, cell2, cell->radius + cell2->radius + 2, 0.01);
    cell2->type = DEFAULT;
    cell2->color = Qt::green;
    cell2->mark = bmark;
    cell2->speed = cell->speed;

    vector<QColor> colors = {Qt::green, Qt::red, Qt::yellow, Qt::gray};
    cell2->color = colors[bmark];
}

void World::optimizeJoints(Physics &physics) {
    vector<Cell*> plant;
    for (auto &cell : physics.cells)
        if (cell.type == DEFAULT)
            plant.push_back(&cell);

    physics.joints.clear();
    unordered_set<int> q;
    for (auto &cell : plant) {
        for (auto &cell2 : plant) {
            auto hash = cell->hash() ^ cell2->hash();

            double dist = cell->radius + cell2->radius;
            if (*cell != *cell2 && cell->distTo(*cell2) - dist < 6 && !q.count(hash)) {
                q.insert(hash);

                double radius = min(max(cell->distTo(*cell2), dist + 2), dist + 4);
                physics.addJoint(cell, cell2, radius, 0.02);
            }
        }
    }
}

void World::draw(QPainter &painter) {
    auto pos1 = cameraPosition * scale,
         pos2 = (cameraPosition + Vect(xsize, 0)) * scale,
         pos3 = (cameraPosition + Vect(xsize, ysize)) * scale,
         pos4 = (cameraPosition + Vect(0, ysize)) * scale;

    painter.setPen(QPen(QBrush(Qt::black), 0.2));
    painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
    painter.drawLine(pos2.x, pos2.y, pos3.x, pos3.y);
    painter.drawLine(pos3.x, pos3.y, pos4.x, pos4.y);
    painter.drawLine(pos4.x, pos4.y, pos1.x, pos1.y);

    for(auto &cell : physics.cells)
        draw(cell, painter);
    for(auto &joint : physics.joints)
        draw(joint, painter);
}

void World::mousePress(int x, int y) {
    mousePosition.set(x, y);
    clickPos.set(x, y);
}

void World::mouseMove(int x, int y) {
    if (mousePosition != Vect(0, 0)) {
        cameraPosition += (Vect(x, y) - mousePosition) / scale;
        mousePosition.set(x, y);
    }
    if (clickPos.distTo(x, y) > 5)
        clickPos.set(0, 0);
}

void World::mouseRelease(int x, int y) {
    mousePosition.set(0, 0);
    if (clickPos.distTo(x, y) < 5) {
        selectedCell = nullptr;
        for (auto &cell : physics.cells) {
            if (cell.distTo(clickPos) < cell.radius)
                selectedCell = &cell;
        }
    }
}

void World::keyPress(int key) {

}

void World::wheel(double delta) {
    double lscale = scale;
    scale *= pow(1.2, delta);
    cameraPosition += Vect(330, 330) * (1 / scale - 1 / lscale);
}
