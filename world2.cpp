#include "world2.h"



World2::World2() {
    ofstream defaultSave("defaultSave.txt");
    defaultSave << QuadConfiguration().toString() << endl;
    defaultSave.close();

    shared_ptr<QuadConfiguration> conf = make_shared<QuadConfiguration> ();
    conf->hpFromMeat = 80;

    for (size_t i = 0; i < 10000; i++) {
        auto &quad = quads[rand() % xsize][rand() % ysize];
        quad.type = MEAT;
        quad.color = 0x00ff00;
        quad.number = 1;
    }

    for (size_t i = 0; i < 10000; i++) {
        auto &quad = quads[rand() % xsize][rand() % ysize];
        quad = Quad<Types>(conf);
        quad.type = BOT;
        quad.color = 0xff0000;
        quad.nn = NN({12, 15, 10, 8});
        quad.nn.addRandom(1);
        quad.mem = {0, 0, 0, 0};
        quad.hp = 20;
        quad.conf = conf;
    }

    for (size_t i = 0; i < xsize * ysize / 4; i++) {
        auto &quad = quads[rand() % xsize][rand() % ysize];
        quad.type = WALL;
        quad.color = 0x999999;
    }





    /*ifstream inp("save.txt");

    string s((istreambuf_iterator<char>(inp)),
              istreambuf_iterator<char>());

    conf.fromString(s);

    cout << conf.toString() << endl;*/
}

void World2::updateQuad(const size_t x, const size_t y) {
    auto &quad = quads[x][y];
    if (quad.isUpdated) {
        quad.isUpdated = false;
    } else if (quad.type == BOT) {
        quad.color = hsv2rgb(quad.number * 150 + 180, 255, min(quad.hp * 2 + 150, 255) * 0 + 255);

        quad.hpFromMeat -= rand() * (1 + quad.conf->hpFromMeatDecreaseRate * 2) / RAND_MAX;

        quad.hp -= quad.conf->hpLosePerTick;
        if (quad.hp <= 0) {
            quad.type = EMPTY;
            quad.color = 0xffffff;
            return;
        }

        int &angle = quad.angle;

        auto &firstLayer = quad.conf->firstLayer;
        for (size_t i = 0; i < firstLayer.size(); i += 3) {
            auto &quad2 = quads.getFromAngle(x, y, angle + firstLayer[i+1], firstLayer[i+2]);
            if (firstLayer[i] == ID_MAP) {
                quad.nn.input.push_back(static_cast<float> (quad2.type) / 4 - 1);
            } else if (firstLayer[i] == NUMBER_MAP) {
                quad.nn.input.push_back(quad2.number);
            } else if (firstLayer[i] == HP_MAP) {
                quad.nn.input.push_back(static_cast<float> (quad2.hp) / 20 - 1);
            } else if (firstLayer[i] == HP_FROM_MEAT_MAP) {
                quad.nn.input.push_back(static_cast<float> (quad2.hpFromMeat) / 5 - 1);
            } else {
                cerr << "No command: " << firstLayer[i] << endl;
            }
        }
        for (auto &i : quad.mem)
            quad.nn.input.push_back(i);

        const auto output = quad.nn.run();
        for (size_t i = 0; i < quad.conf->memoryCells; i++)
            quad.mem[i] = quad.mem[i] * 0.9 + output[i + output.size() - quad.conf->memoryCells] * 0.1;

        quad.number = output[output.size() - quad.conf->memoryCells - 1];

        auto &lastLayer = quad.conf->lastLayer;
        for (size_t i = 0; i < lastLayer.size(); i += 3) {
            const int x2 = x + quads.getXFromAngle(quad.angle + lastLayer[i+1]) * lastLayer[i+2],
                      y2 = y + quads.getYFromAngle(quad.angle + lastLayer[i+1]) * lastLayer[i+2];
            auto &quad2 = quads.get(x2, y2);
            if (lastLayer[i] == MOVE) {
                if (lastLayer[i+2]) { // if length of movement != 0
                    if (output[i / 3] > 0 && (quad2.type == EMPTY || quad2.type == MEAT || quad2.type == BOT)) {
                        if (quad2.type == MEAT) {
                            quad2 = Quad<Types>();
                            quad2.color = 0xffffff;
                            quad.hp += quad.hpFromMeat;
                        } else if (quad2.type == BOT) {
                            const int toTransfer = min(quad2.hp, quad2.conf->hpLosePerAttack);
                            quad2.hp -= toTransfer;
                            quad.hp += toTransfer * quad.conf->hpGainPerAttack / quad2.conf->hpLosePerAttack;
                            return;
                        }
                        quad.hp -= quad.conf->hpLosePerMove;
                        quads.swap(x, y, x2, y2);
                        return;
                    }
                } else {
                    if (output[i / 3] > 0)
                        quad.rotate(lastLayer[i+1]);
                }
            } else if (lastLayer[i] == SPLIT) {
                if (output[i / 3] > 0 && quad.hp >= 2 && quad2.type == EMPTY) {
                    quad.hp = quad.hp * quad.conf->hpSplitMultiplier / 2;
                    quad2 = quad;

                    quad2.hpFromMeat = quad2.conf->hpFromMeat;
                    quad2.nn.addRandom(quad2.conf->mutateFromParent);
                    for (auto &i : quad2.mem)
                        i = 0;
                }
            } else {
                cerr << "No command: " << lastLayer[i] << endl;
            }
        }
    }

}

void World2::update() {
    for (size_t x = 0; x < xsize; x++) {
        for (size_t y = 0; y < ysize; y++) {
            updateQuad(x, y);
        }
    }

    for (size_t i = 0; i < 8; i++) {
        auto &quad = quads[rand() % xsize][rand() % ysize];
        if (quad.type == EMPTY) {
            quad.type = MEAT;
            quad.color = 0x00ff00;
            quad.number = 1;
        }
    }
}

void World2::draw(QPainter &painter) {
    const Vect cameraPos = Vect(int(cameraPosition.x * scale), int(cameraPosition.y * scale));
    auto pos1 = cameraPos,
         pos2 = cameraPos + Vect(xsize, 0) * scale,
         pos3 = cameraPos + Vect(xsize, ysize) * scale,
         pos4 = cameraPos + Vect(0, ysize) * scale;

    for (size_t x = 0; x < xsize; x++) {
        for (size_t y = 0; y < ysize; y++) {
            /*const double radius = 0.5;
            Vect worldPos = cameraPos + Vect(x, y) * scale + Vect(scale, scale) * (1 - radius) / 2;
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::GlobalColor(Qt::yellow));
            painter.drawEllipse(worldPos.x, worldPos.y, scale * radius, scale * radius);*/
            Vect worldPos = cameraPos + Vect(x, y) * scale;
            painter.setPen(Qt::NoPen);
            painter.setBrush(quads[x][y].color);
            painter.drawRect(worldPos.x, worldPos.y, scale, scale);

        }
    }

    painter.setPen(QPen(QBrush(Qt::black), 0.4));
    /*painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
    painter.drawLine(pos2.x, pos2.y, pos3.x, pos3.y);
    painter.drawLine(pos3.x, pos3.y, pos4.x, pos4.y);
    painter.drawLine(pos4.x, pos4.y, pos1.x, pos1.y);*/

    for (size_t x = 0; x <= xsize; x++)
        painter.drawLine(cameraPos.x + x * scale, cameraPos.y, cameraPos.x + x * scale, cameraPos.y + ysize * scale);
    for (size_t y = 0; y <= ysize; y++)
        painter.drawLine(cameraPos.x, cameraPos.y + y * scale, cameraPos.x + xsize * scale, cameraPos.y + y * scale);
}

void World2::mousePress(int x, int y) {
    mousePosition.set(x, y);
    clickPos.set(x, y);
}

void World2::mouseMove(int x, int y) {
    if (mousePosition != Vect(0, 0)) {
        cameraPosition += (Vect(x, y) - mousePosition) / scale;
        mousePosition.set(x, y);
    }
    if (clickPos.distTo(x, y) > 5)
        clickPos.set(0, 0);
}

void World2::mouseRelease(int x, int y) {
    mousePosition.set(0, 0);
    if (clickPos.distTo(x, y) < 5) { // click
    }
}

void World2::keyPress(int key) {

}

void World2::wheel(double delta) {
    double lscale = scale;
    //scale *= pow(1.2, delta);
    scale = int(scale + (delta * scale + 5) / 10);
    if (lscale == scale) {
        scale += (delta > 0) ? 1 : -1;
    }
    scale = max(scale, 3.0);

    cameraPosition += Vect(330, 330) * (1 / scale - 1 / lscale);
}
