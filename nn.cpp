#include "nn.h"

NN::NN(vector<int> arch) : arch(arch) {
    weights = vector<float> (calculateArch());
}

NN* NN::cloneFrom(NN &nn) {
    arch = nn.arch;
    weights = nn.weights;

    return this;
}

float NN::activation(float x) {
    return x / sqrt(1 + x * x);
}

int NN::calculateArch() {
    int n = 1;
    for(int i=1; i < arch.size(); i++)
        n += arch[i-1] * arch[i] + arch[i];
    return n;
}

const vector<float>& NN::run() {
    int weightId = 0;
    for(int lvl=1; lvl < arch.size(); lvl++) {
        output.resize(arch[lvl], 0);
        for(int j=0; j < arch[lvl]; j++) {
            for(int i=0; i < arch[lvl-1]; i++) {
                output[j] += input[i] * weights[weightId];
                weightId++;
            }
            output[j] = activation(output[j] + weights[weightId]);
            weightId++;
        }

        input = output;
    }
    swap(input, output);
    input.clear();
    return output;
}

vector<float> NN::run(vector<float> input) {
    int weightId = 0;
    for(int lvl=1; lvl < arch.size(); lvl++) {
        vector<float> output(arch[lvl]);
        for(int j=0; j < arch[lvl]; j++) {
            for(int i=0; i < arch[lvl-1]; i++) {
                output[j] += input[i] * weights[weightId];
                weightId++;
            }
            output[j] = activation(output[j] + weights[weightId]);
            weightId++;
        }

        input = output;
    }
    return input;
}

void NN::addRandom(float range) {
    for (size_t i=0; i < weights.size(); i++) {
        weights[i] += static_cast<float> (rand()) * range * 2.0 / static_cast<float> (RAND_MAX) - range;
    }
}
