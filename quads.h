#ifndef QUADS_H
#define QUADS_H

using namespace std;
#include <vector>
#include "nn.h"
#include <QColor>
#include <sstream>
#include <string>
#include <memory>

#include <iostream>

enum neuronType {
    ID_MAP = 0,
    NUMBER_MAP,
    HP_MAP,
    HP_FROM_MEAT_MAP,
    MOVE,
    SPLIT
};

struct QuadConfiguration {
    vector<int> nnLayers = {15, 10};

    vector<string> neuronTypes = {
        "ID_MAP",
        "NUMBER_MAP",
        "HP_MAP",
        "HP_FROM_MEAT_MAP",
        "MOVE",
        "SPLIT"
    };

    size_t memoryCells = 4;
    size_t hpFromMeat = 20;
    float hpFromMeatDecreaseRate = 0.01;
    float mutateFromParent = 0.2;
    float hpSplitMultiplier = 0.8;
    int hpLosePerTick = 1;
    int hpLosePerMove = 1;

    int hpGainPerAttack = 20;
    int hpLosePerAttack = 20;

    vector<int> firstLayer = {HP_MAP, 0, 0,
                              NUMBER_MAP, -1, 1,
                              NUMBER_MAP, 0, 1,
                              NUMBER_MAP, 1, 1,
                              NUMBER_MAP, -1, 2,
                              NUMBER_MAP, 0, 2,
                              NUMBER_MAP, 1, 2,
                              HP_FROM_MEAT_MAP, 0, 0};

    vector<int> lastLayer = {MOVE, -1, 0,
                             MOVE, 1, 0,
                             MOVE, 0, 1,
                             SPLIT, 0, 1};

    string toString() {
        stringstream s;
        s << "memoryCells = " << memoryCells << endl;
        s << "hpFromMeat = " << hpFromMeat << endl;
        s << "hpFromMeatDecreaseRate = " << hpFromMeatDecreaseRate << endl;
        s << "mutateFromParent = " << mutateFromParent << endl;
        s << "hpSplitMultiplier = " << hpSplitMultiplier << endl;
        s << "hpLosePerTick = " << hpLosePerTick << endl;
        s << "hpLosePerMove = " << hpLosePerMove << endl;

        s << "hpGainPerAttack = " << hpGainPerAttack << endl;
        s << "hpLosePerAttack = " << hpLosePerAttack << endl;

        s << "\n// input and output layer should be in format:\n"
          << "// MODIFIER angle length\n"
          << "// where MODIFIER:\n"
          << "// ID_MAP, NUMBER_MAP, HP_MAP, HP_FROM_MEAT_MAP, MOVE, SPLIT\n";

        s << "\n// input layer\n";
        s << "firstLayer =\n";
        for (size_t i = 0; i < firstLayer.size(); i += 3) {
            s << neuronTypes[firstLayer[i]] << ' ' << firstLayer[i+1] << ' ' << firstLayer[i+2] << '\n';
        }
        s << endl;


        s << "// middle layers, each number defines number of neurons on each layer\n"
          << "nnLayers = \n";
        for (size_t i = 0; i < nnLayers.size(); i++) {
            s << nnLayers[i] << ' ';
        }
        s << endl;

        s << "\n// output layer\n";
        s << "lastLayer =\n";
        for (size_t i = 0; i < lastLayer.size(); i += 3) {
            s << neuronTypes[lastLayer[i]] << ' ' << lastLayer[i+1] << ' ' << lastLayer[i+2] << '\n';
        }
        s << endl;

        return s.str();
    }

    bool isNumber(const string &s) {
        for (const auto ch : s) {
            if ( !(ch == '-' || (ch >= '0' && ch <= '9')) )
                return false;
        }
        return true;
    }
    int getNeuronType(const string &s) {
        for (size_t i = 0; i < neuronTypes.size(); i++) {
            if (neuronTypes[i] == s)
                return i;
        }
        return -1;
    }
    void fromString(string str) {
        vector<string> s;

        string tmp;
        bool comment = false;
        for (auto ch : str) {
            if (ch == '/') {
                comment = true;
            } else if (comment && '\n') {
                comment = false;
                tmp = "";
            } else if (comment) {
                // nothing to do here
            } if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '=') {
                if (tmp.size()) {
                    s.push_back(tmp);
                    tmp = "";
                }
            } else {
                tmp += ch;
            }
        }
        if (tmp.size()) {
            s.push_back(tmp);
        }
        for (size_t i = 0; i + 1 < s.size(); i++) {
            if (s[i] == "memoryCells") {
                memoryCells = stoi(s[i+1]);
            } else if (s[i] == "hpFromMeat") {
                hpFromMeat = stoi(s[i+1]);
            } else if (s[i] == "hpFromMeatDecreaseRate") {
                hpFromMeatDecreaseRate = stod(s[i+1]);
            } else if (s[i] == "mutateFromParent") {
                mutateFromParent = stod(s[i+1]);
            } else if (s[i] == "hpSplitMultiplier") {
                hpSplitMultiplier = stod(s[i+1]);
            } else if (s[i] == "hpLosePerTick") {
                hpLosePerTick = stoi(s[i+1]);
            } else if (s[i] == "hpLosePerMove") {
                hpLosePerMove = stoi(s[i+1]);
            } else if (s[i] == "hpGainPerAttack") {
                hpGainPerAttack = stoi(s[i+1]);
            } else if (s[i] == "hpLosePerAttack") {
                hpLosePerAttack = stoi(s[i+1]);
            } else if (s[i] == "firstLayer" || s[i] == "nnLayers" || s[i] == "lastLayer") {
                vector<int> &layer = (s[i] == "firstLayer") ? firstLayer : ((s[i] == "nnLayers") ? nnLayers : lastLayer);
                layer.clear();

                for(i++; i < s.size(); i++) {
                    if (getNeuronType(s[i]) != -1) {
                        layer.push_back(getNeuronType(s[i]));
                    } else if(isNumber(s[i])) {
                        layer.push_back(stoi(s[i]));
                    } else {
                        i--;
                        break;
                    }
                }
            }
        }
    }
};

template<typename TYPE> class Quads;

template<typename TYPE> class Quad {
public:
    Quad() {}
    Quad(shared_ptr<QuadConfiguration> conf) {
        color = 0xff0000;
        type = TYPE::BOT;

        hpFromMeat = conf->hpFromMeat;

        vector<int> arch = {static_cast<int> (conf->memoryCells + conf->firstLayer.size() / 3)};
        for (auto i : conf->nnLayers)
            arch.push_back(i);
        arch.push_back(conf->memoryCells + conf->lastLayer.size() / 3 + 1);

        nn = NN(arch);
        nn.addRandom(1);
        mem = vector<float> (conf->memoryCells);
        this->conf = conf;
    }

    QColor color = 0xffffff;
    TYPE type = static_cast<TYPE> (0);
    bool isUpdated = false;

    float number = 0;
    int hpFromMeat = 20;
    int hp = 60;
    int angle = 0;
    void rotate(const int dAngle) {
        angle = (angle + dAngle + 8) % 8;
    }
    NN nn;
    vector<float> mem;
    shared_ptr<QuadConfiguration> conf = nullptr;
};


template<typename TYPE> class Quads {
private:
    vector<vector< Quad<TYPE> >> quads;
public:
    Quads(const size_t x, const size_t y) {
        quads = vector<vector< Quad<TYPE> >> (x, vector< Quad<TYPE> > (y));
    }

    vector< Quad<TYPE> >& operator[] (const size_t i) {
        return quads[i];
    }

    Quad<TYPE>& get(const int x, const int y) { // cycled access
        return quads[(x + quads.size()   ) % quads.size()]
                    [(y + quads[0].size()) % quads[0].size()];
    }

    static constexpr int ax[] = {1, 1, 0, -1, -1, -1, 0, 1};
    static constexpr int ay[] = {0, 1, 1, 1, 0, -1, -1, -1};

    int getXFromAngle(const int angle) const {
        return ax[angle];
    }
    int getYFromAngle(const int angle) const {
        return ay[angle];
    }
    Quad<TYPE>& getFromAngle(const int x, const int y, const int angle, const int len = 1) { // cycled access
        return get(x + ax[(angle + 8) % 8] * len, y + ay[(angle + 8) % 8] * len);
    }

    void swap(const int x, const int y, const int tx, const int ty) {
        std::swap(get(x, y), get(tx, ty));
        if (tx > x || (tx == x && ty > y)) {
            get(tx, ty).isUpdated = true;
        } else {
            get(x, y).isUpdated = true;
        }
    }
};

#endif // QUADS_H
