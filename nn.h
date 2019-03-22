#ifndef NN_H
#define NN_H

#include <vector>
#include <math.h>
#include <random>
using namespace std;

class NN {
public:
    vector<int> arch;
    vector<float> weights;

    vector<float> input, output;

    NN() {};
    NN(vector<int> arch);
    float activation(float x);
    int calculateArch();
    const vector<float>& run();
    vector<float> run(vector<float> input);
    void addRandom(float range);
    NN* cloneFrom(NN &nn);
};

#endif // NN_H
