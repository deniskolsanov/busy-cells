#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <vect.h>
#include <vector>
#include <QPainter>

struct N {
    float capacity = 1;
    Vect vect;
    float val = 0;
    int timeActivated = 0;
};


class Test {
public:
    vector< pair<int, int> > sides{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    vector< vector<N> > q;

    int blinkPeriod = 10;

    Test() {
        q = vector< vector<N> >(10, vector<N> (10));

        for (int i=4; i < 6; i++) {
            q[1][i].val = 1;
            q[2][i].val = 1;
        }
    }

    void runNeuron(int x, int y) {
        auto &me = q[x][y];

        if (me.timeActivated > 0) {
            me.timeActivated++;
            me.val = 0;
            float output = 0.5 - abs(me.timeActivated - blinkPeriod / 2) / blinkPeriod / 2 / 2;

            float sum = 0;
            for (auto &side : sides) {
                auto &other = q[x+side.first][y+side.second];
                sum += min(output, float((1 - other.val) * 0.1));
            }
            for (auto &side : sides) {
                auto &other = q[x+side.first][y+side.second];
                float diff = min(output, float((1 - other.val) * 0.1)) * output / sum;
                other.val += diff;
            }

            if (me.timeActivated > blinkPeriod)
                me.timeActivated = 0;
        } else {
            me.val *= 0.95;
            if (me.val > 0.8) {
                me.timeActivated = 1;
            }
        }
    }

    void test(QPainter &painter) {
        for (int x=1; x < 9; x++) {
            for (int y=1; y < 9; y++) {
                runNeuron(x, y);
            }
        }

        for (int x=0; x < 400; x++) {
            for (int y=0; y < 400; y++) {
                painter.setPen(QPen(QBrush(QColor( q[x/40][y/40].val * 255.0, 0, 0 )), 1));
                painter.drawPoint(100 + x, 100 + y);
            }
        }
    }
};


#endif // TEST_H
